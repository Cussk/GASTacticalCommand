//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFBuildingProductionComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Actors/TCFBuildingActor.h"
#include "Actors/TCFSquadActor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/TCFAffiliationComponent.h"
#include "Data/TCFBuildingDefinition.h"
#include "Data/TCFProductionCatalogDefinition.h"
#include "Data/TCFProductionOptionDefinition.h"
#include "Data/TCFSquadDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TCFPlayerState.h"
#include "Subsystems/TCFRelationshipSubsystem.h"
#include "TCFGameplayTags.h"
#include "Components/TCFPlayerResourceBankComponent.h"

UTCFBuildingProductionComponent::UTCFBuildingProductionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFBuildingProductionComponent::BeginPlay()
{
	Super::BeginPlay();

	BuildingOwner = Cast<ATCFBuildingActor>(GetOwner());
}

void UTCFBuildingProductionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopProductionTimer();
	
	Super::EndPlay(EndPlayReason);
}

void UTCFBuildingProductionComponent::GetProductionOptions(
	TArray<UTCFProductionOptionDefinition*>& OutOptions) const
{
	OutOptions.Reset();

	const UTCFProductionCatalogDefinition* Catalog = GetProductionCatalog();
	if (!Catalog)
	{
		return;
	}

	Catalog->GetProductionOptions(OutOptions);
}

bool UTCFBuildingProductionComponent::CanUseProductionOption(
	UTCFProductionOptionDefinition* ProductionOption,
	ATCFPlayerState* RequestingPlayerState) const
{
	if (!BuildingOwner || !BuildingOwner->IsActive())
	{
		return false;
	}

	if (!BuildingOwner->GetBuildingRoleTags().HasTagExact(TCFGameplayTags::Building_Role_Production))
	{
		return false;
	}

	if (!IsValid(ProductionOption) || !ProductionOption->IsValidProductionOption())
	{
		return false;
	}

	const UTCFProductionCatalogDefinition* Catalog = GetProductionCatalog();
	if (!Catalog || !Catalog->ContainsProductionOption(ProductionOption))
	{
		return false;
	}

	if (!IsRequesterOwner(RequestingPlayerState))
	{
		return false;
	}

	if (!DoesRequesterMeetCommanderTagRequirements(*ProductionOption, *RequestingPlayerState))
	{
		return false;
	}

	const TSubclassOf<ATCFSquadActor> SquadClass =
		ProductionOption->GetResolvedSquadActorClass(DefaultSquadActorClass);

	return SquadClass != nullptr;
}

bool UTCFBuildingProductionComponent::RequestProduction(
	UTCFProductionOptionDefinition* ProductionOption,
	ATCFPlayerState* RequestingPlayerState)
{
	if (HasPendingProductionRequest() || !HasProductionQueueSpace())
	{
		return false;
	}

	if (!CanUseProductionOption(ProductionOption, RequestingPlayerState))
	{
		return false;
	}

	PendingProductionRequest.ProductionOption = ProductionOption;
	PendingProductionRequest.RequestingPlayerState = RequestingPlayerState;

	if (!TryActivateProductionAbility())
	{
		ClearPendingProductionRequest();
		return false;
	}

	return true;
}

bool UTCFBuildingProductionComponent::HasPendingProductionRequest() const
{
	return PendingProductionRequest.IsValid();
}

const FTCFProductionRequest& UTCFBuildingProductionComponent::GetPendingProductionRequest() const
{
	return PendingProductionRequest;
}

void UTCFBuildingProductionComponent::ClearPendingProductionRequest()
{
	PendingProductionRequest = FTCFProductionRequest();
}

bool UTCFBuildingProductionComponent::CanExecutePendingProductionRequest() const
{
	return HasPendingProductionRequest()
		&& CanUseProductionOption(
			PendingProductionRequest.ProductionOption,
			PendingProductionRequest.RequestingPlayerState);
}

UTCFPlayerResourceBankComponent* UTCFBuildingProductionComponent::GetResourceBankForPendingRequest() const
{
	const ATCFPlayerState* RequestingPlayerState = PendingProductionRequest.RequestingPlayerState;
	return RequestingPlayerState
		? RequestingPlayerState->GetPlayerResourceBankComponent()
		: nullptr;
}

void UTCFBuildingProductionComponent::InitializeProductionAbility()
{
	if (!BuildingOwner || !BuildingOwner->HasAuthority() || !ProductionAbilityClass)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystem = BuildingOwner->GetAbilitySystemComponent();
	if (!AbilitySystem || ProductionAbilityHandle.IsValid())
	{
		return;
	}

	FGameplayAbilitySpec AbilitySpec(ProductionAbilityClass, 1);
	ProductionAbilityHandle = AbilitySystem->GiveAbility(AbilitySpec);
}

bool UTCFBuildingProductionComponent::EnqueuePendingProduction()
{
	if (!CanExecutePendingProductionRequest() || !HasProductionQueueSpace())
	{
		return false;
	}

	const UTCFProductionOptionDefinition* ProductionOption = PendingProductionRequest.ProductionOption;
	if (!ProductionOption)
	{
		return false;
	}

	FTCFProductionQueueItem QueueItem;
	QueueItem.ProductionOption = PendingProductionRequest.ProductionOption;
	QueueItem.RequestingPlayerState = PendingProductionRequest.RequestingPlayerState;
	QueueItem.CompletedProductionWork = 0.0f;
	QueueItem.RequiredProductionWork = ProductionOption->GetSafeProductionTime();

	ProductionQueue.Add(QueueItem);
	OnProductionQueueChanged.Broadcast(this);
	StartProductionTimerIfNeeded();

	return true;
}

bool UTCFBuildingProductionComponent::HasProductionQueueSpace() const
{
	const int32 EffectiveMaxQueueSize = GetEffectiveMaxQueueSize();
	return EffectiveMaxQueueSize <= 0 || ProductionQueue.Num() < EffectiveMaxQueueSize;
}

bool UTCFBuildingProductionComponent::HasQueuedProduction() const
{
	return ProductionQueue.Num() > 0;
}

int32 UTCFBuildingProductionComponent::GetProductionQueueCount() const
{
	return ProductionQueue.Num();
}

int32 UTCFBuildingProductionComponent::GetEffectiveMaxQueueSize() const
{
	return MaxQueueSize + BonusQueueSize;
}

void UTCFBuildingProductionComponent::GetProductionQueue(
	TArray<FTCFProductionQueueItem>& OutQueue) const
{
	OutQueue = ProductionQueue;
}

bool UTCFBuildingProductionComponent::TryGetActiveProductionItem(
	FTCFProductionQueueItem& OutQueueItem) const
{
	if (ProductionQueue.IsEmpty())
	{
		OutQueueItem = FTCFProductionQueueItem();
		return false;
	}

	OutQueueItem = ProductionQueue[0];
	return OutQueueItem.IsValid();
}

float UTCFBuildingProductionComponent::GetActiveProductionProgressAlpha() const
{
	FTCFProductionQueueItem ActiveItem;
	return TryGetActiveProductionItem(ActiveItem)
		? ActiveItem.GetProgressAlpha()
		: 0.0f;
}

bool UTCFBuildingProductionComponent::TryActivateProductionAbility()
{
	if (!BuildingOwner)
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystem = BuildingOwner->GetAbilitySystemComponent();
	if (!AbilitySystem)
	{
		return false;
	}

	if (!ProductionAbilityHandle.IsValid())
	{
		InitializeProductionAbility();
	}

	return ProductionAbilityHandle.IsValid()
		&& AbilitySystem->TryActivateAbility(ProductionAbilityHandle);
}

bool UTCFBuildingProductionComponent::DoesRequesterMeetCommanderTagRequirements(
	const UTCFProductionOptionDefinition& ProductionOption,
	const ATCFPlayerState& RequestingPlayerState) const
{
	const UAbilitySystemComponent* CommanderASC = RequestingPlayerState.GetAbilitySystemComponent();
	if (!CommanderASC)
	{
		return ProductionOption.RequiredCommanderTags.IsEmpty()
			&& ProductionOption.BlockedCommanderTags.IsEmpty();
	}

	FGameplayTagContainer OwnedTags;
	CommanderASC->GetOwnedGameplayTags(OwnedTags);

	if (!OwnedTags.HasAll(ProductionOption.RequiredCommanderTags))
	{
		return false;
	}

	return ProductionOption.BlockedCommanderTags.IsEmpty()
		|| !OwnedTags.HasAny(ProductionOption.BlockedCommanderTags);
}

void UTCFBuildingProductionComponent::RefundPendingProductionCost()
{
	UTCFPlayerResourceBankComponent* ResourceBank = GetResourceBankForPendingRequest();
	const UTCFProductionOptionDefinition* ProductionOption = PendingProductionRequest.ProductionOption;

	if (ResourceBank && ProductionOption)
	{
		ResourceBank->AddResources(ProductionOption->Cost);
	}
}

bool UTCFBuildingProductionComponent::TrySpawnProducedSquad(
	UTCFProductionOptionDefinition* ProductionOption,
	ATCFPlayerState* RequestingPlayerState,
	ATCFSquadActor*& OutSquad)
{
	OutSquad = nullptr;

	if (!CanUseProductionOption(ProductionOption, RequestingPlayerState))
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const TSubclassOf<ATCFSquadActor> SquadClass =
		ProductionOption->GetResolvedSquadActorClass(DefaultSquadActorClass);

	const FTransform SpawnTransform = ResolveSpawnTransform(*ProductionOption);

	ATCFSquadActor* SpawnedSquad = World->SpawnActorDeferred<ATCFSquadActor>(
		SquadClass,
		SpawnTransform,
		BuildingOwner,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (!SpawnedSquad)
	{
		return false;
	}

	SpawnedSquad->SetOwner(RequestingPlayerState);
	SpawnedSquad->ApplySquadDefinition(ProductionOption->GetSquadDefinition());
	ApplySpawnedSquadAffiliation(*SpawnedSquad, *RequestingPlayerState);

	UGameplayStatics::FinishSpawningActor(SpawnedSquad, SpawnTransform);

	OutSquad = SpawnedSquad;
	OnProducedSquadSpawned.Broadcast(SpawnedSquad, ProductionOption);

	return true;
}

void UTCFBuildingProductionComponent::StartProductionTimerIfNeeded()
{
	if (!BuildingOwner || !BuildingOwner->HasAuthority() || ProductionQueue.IsEmpty())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World || World->GetTimerManager().IsTimerActive(ProductionTimerHandle))
	{
		return;
	}

	World->GetTimerManager().SetTimer(
		ProductionTimerHandle,
		this,
		&UTCFBuildingProductionComponent::HandleProductionTimerTick,
		ProductionUpdateInterval,
		true);
}

void UTCFBuildingProductionComponent::StopProductionTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ProductionTimerHandle);
	}
}

void UTCFBuildingProductionComponent::HandleProductionTimerTick()
{
	if (!BuildingOwner || !BuildingOwner->HasAuthority())
	{
		StopProductionTimer();
		return;
	}

	if (ProductionQueue.IsEmpty())
	{
		StopProductionTimer();
		return;
	}

	AdvanceActiveProduction(ProductionUpdateInterval);

	if (ProductionQueue.IsEmpty())
	{
		StopProductionTimer();
	}
}

void UTCFBuildingProductionComponent::AdvanceActiveProduction(float DeltaSeconds)
{
	if (ProductionQueue.IsEmpty())
	{
		return;
	}

	FTCFProductionQueueItem& ActiveItem = ProductionQueue[0];
	if (!ActiveItem.IsValid())
	{
		ProductionQueue.RemoveAt(0);
		OnProductionQueueChanged.Broadcast(this);
		return;
	}

	const float ProductionRateMultiplier = GetProductionRateMultiplier(ActiveItem);
	ActiveItem.CompletedProductionWork += FMath::Max(0.0f, DeltaSeconds) * ProductionRateMultiplier;

	if (ActiveItem.IsComplete())
	{
		CompleteActiveProduction();
	}
	
	OnProductionProgressChanged.Broadcast(this, ActiveItem.GetProgressAlpha());
}

void UTCFBuildingProductionComponent::CompleteActiveProduction()
{
	if (ProductionQueue.IsEmpty())
	{
		return;
	}

	const FTCFProductionQueueItem CompletedItem = ProductionQueue[0];
	ProductionQueue.RemoveAt(0);
	OnProductionQueueChanged.Broadcast(this);

	ATCFSquadActor* SpawnedSquad = nullptr;
	SpawnProductionQueueItem(CompletedItem, SpawnedSquad);
}

bool UTCFBuildingProductionComponent::SpawnProductionQueueItem(
	const FTCFProductionQueueItem& QueueItem,
	ATCFSquadActor*& OutSquad)
{
	OutSquad = nullptr;

	if (!QueueItem.IsValid())
	{
		return false;
	}

	return TrySpawnProducedSquad(
		QueueItem.ProductionOption,
		QueueItem.RequestingPlayerState,
		OutSquad);
}

float UTCFBuildingProductionComponent::GetProductionRateMultiplier(
	const FTCFProductionQueueItem& QueueItem) const
{
	return 1.0f;
}

UTCFProductionCatalogDefinition* UTCFBuildingProductionComponent::GetProductionCatalog() const
{
	const UTCFBuildingDefinition* BuildingDefinition = BuildingOwner
		? BuildingOwner->GetBuildingDefinition()
		: nullptr;

	return BuildingDefinition ? BuildingDefinition->ProductionCatalog : nullptr;
}

bool UTCFBuildingProductionComponent::IsRequesterOwner(const ATCFPlayerState* RequestingPlayerState) const
{
	if (!BuildingOwner || !RequestingPlayerState)
	{
		return false;
	}

	const UWorld* World = GetWorld();
	const UTCFRelationshipSubsystem* RelationshipSubsystem = World
		? World->GetSubsystem<UTCFRelationshipSubsystem>()
		: nullptr;

	if (!RelationshipSubsystem)
	{
		return false;
	}

	return RelationshipSubsystem->GetActorRelationship(RequestingPlayerState, BuildingOwner) == ETCFSquadRelationship::Own;
}

FTransform UTCFBuildingProductionComponent::ResolveSpawnTransform(
	const UTCFProductionOptionDefinition& ProductionOption) const
{
	const FVector BuildingLocation = BuildingOwner
		? BuildingOwner->GetActorLocation()
		: FVector::ZeroVector;

	const FRotator BuildingRotation = BuildingOwner
		? BuildingOwner->GetActorRotation()
		: FRotator::ZeroRotator;

	FVector SpawnDirection = BuildingOwner
		? BuildingOwner->GetActorForwardVector()
		: FVector::ForwardVector;

	SpawnDirection.Z = 0.0f;
	if (SpawnDirection.IsNearlyZero())
	{
		SpawnDirection = FVector::ForwardVector;
	}
	else
	{
		SpawnDirection.Normalize();
	}

	float InteractionRadius = 200.0f;
	if (BuildingOwner)
	{
		if (const UPrimitiveComponent* InteractionComponent = BuildingOwner->GetInteractionCollisionComponent())
		{
			InteractionRadius = InteractionComponent->Bounds.BoxExtent.Size2D();
		}
	}

	float SquadRadius = 100.0f;
	if (const UTCFSquadDefinition* SquadDefinition = ProductionOption.GetSquadDefinition())
	{
		SquadRadius = SquadDefinition->SelectionRadius;
	}

	const FVector SpawnLocation =
		BuildingLocation + SpawnDirection * (InteractionRadius + SquadRadius + SpawnPadding);

	return FTransform(BuildingRotation, SpawnLocation, FVector::OneVector);
}

void UTCFBuildingProductionComponent::ApplySpawnedSquadAffiliation(
	ATCFSquadActor& Squad,
	const ATCFPlayerState& RequestingPlayerState) const
{
	const UTCFAffiliationComponent* PlayerAffiliation = RequestingPlayerState.GetAffiliationComponent();
	UTCFAffiliationComponent* SquadAffiliation = Squad.GetAffiliationComponent();

	if (!PlayerAffiliation || !SquadAffiliation)
	{
		return;
	}

	SquadAffiliation->SetAffiliation(PlayerAffiliation->GetAffiliation());
}