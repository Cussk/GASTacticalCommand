//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFBuildingProductionComponent.h"

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

	const TSubclassOf<ATCFSquadActor> SquadClass =
		ProductionOption->GetResolvedSquadActorClass(DefaultSquadActorClass);

	return SquadClass != nullptr;
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