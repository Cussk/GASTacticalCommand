//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Actors/TCFBuildingActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TCFAffiliationComponent.h"
#include "Net/UnrealNetwork.h"
#include "Subsystems/TCFRTSPlacementGridSubsystem.h"

ATCFBuildingActor::ATCFBuildingActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BuildingVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingVisual"));
	BuildingVisual->SetupAttachment(SceneRoot);
	BuildingVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BuildingVisual->SetGenerateOverlapEvents(false);

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(SceneRoot);
	InteractionCollision->SetBoxExtent(FVector(150.0f, 150.0f, 120.0f));
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InteractionCollision->SetGenerateOverlapEvents(false);

	AffiliationComponent = CreateDefaultSubobject<UTCFAffiliationComponent>(TEXT("AffiliationComponent"));
}

void ATCFBuildingActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyDefinitionVisuals();
	ApplyRuntimeStatePresentation();
}

void ATCFBuildingActor::BeginPlay()
{
	Super::BeginPlay();

	InitializeFromDefinition();
	TryReservePlacementFootprint();
}

void ATCFBuildingActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ReleasePlacementFootprint();
	
	Super::EndPlay(EndPlayReason);
}

void ATCFBuildingActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATCFBuildingActor, BuildingDefinition);
	DOREPLIFETIME(ATCFBuildingActor, RuntimeState);
	DOREPLIFETIME(ATCFBuildingActor, ConstructionWorkCompleted);
}

void ATCFBuildingActor::ApplyBuildingDefinition(
	UTCFBuildingDefinition* NewDefinition,
	bool bApplyInitialRuntimeState)
{
	if (!HasAuthority())
	{
		return;
	}

	BuildingDefinition = NewDefinition;

	if (bApplyInitialRuntimeState && BuildingDefinition)
	{
		RuntimeState = BuildingDefinition->InitialRuntimeState;
	}

	InitializeConstructionStateFromDefinition();
	ApplyDefinitionVisuals();
	ApplyRuntimeStatePresentation();
}

UTCFBuildingDefinition* ATCFBuildingActor::GetBuildingDefinition() const
{
	return BuildingDefinition;
}

FGameplayTag ATCFBuildingActor::GetBuildingTypeTag() const
{
	return BuildingDefinition ? BuildingDefinition->BuildingTypeTag : FGameplayTag();
}

FGameplayTagContainer ATCFBuildingActor::GetBuildingRoleTags() const
{
	return BuildingDefinition ? BuildingDefinition->BuildingRoleTags : FGameplayTagContainer();
}

FText ATCFBuildingActor::GetDisplayName() const
{
	return BuildingDefinition ? BuildingDefinition->DisplayName : FText::GetEmpty();
}

const TArray<FTCFResourceAmount>& ATCFBuildingActor::GetCost() const
{
	static const TArray<FTCFResourceAmount> EmptyCost;
	return BuildingDefinition ? BuildingDefinition->Cost : EmptyCost;
}

FIntPoint ATCFBuildingActor::GetFootprintSize() const
{
	return BuildingDefinition ? BuildingDefinition->GetSafeFootprintSize() : FIntPoint(1, 1);
}

ETCFBuildingRuntimeState ATCFBuildingActor::GetRuntimeState() const
{
	return RuntimeState;
}

bool ATCFBuildingActor::HasReservedPlacementFootprint() const
{
	return bHasReservedPlacementFootprint;
}

FIntPoint ATCFBuildingActor::GetReservedPlacementAnchorCell() const
{
	return ReservedPlacementAnchorCell;
}

bool ATCFBuildingActor::IsActive() const
{
	return RuntimeState == ETCFBuildingRuntimeState::Active;
}

void ATCFBuildingActor::SetRuntimeState(ETCFBuildingRuntimeState NewRuntimeState)
{
	if (!HasAuthority() || RuntimeState == NewRuntimeState)
	{
		return;
	}

	const ETCFBuildingRuntimeState OldRuntimeState = RuntimeState;
	RuntimeState = NewRuntimeState;

	ApplyRuntimeStatePresentation();
	OnRuntimeStateChanged.Broadcast(OldRuntimeState, RuntimeState);
}

bool ATCFBuildingActor::IsUnderConstruction() const
{
	return RuntimeState == ETCFBuildingRuntimeState::UnderConstruction;
}

bool ATCFBuildingActor::IsConstructionComplete() const
{
	if (!BuildingDefinition)
	{
		return RuntimeState == ETCFBuildingRuntimeState::Active;
	}

	const float RequiredWork = GetRequiredConstructionWork();
	return RequiredWork <= 0.0f || ConstructionWorkCompleted >= RequiredWork;
}

bool ATCFBuildingActor::CanReceiveConstructionWork() const
{
	return BuildingDefinition
		&& BuildingDefinition->bCanReceiveWorkerConstruction
		&& RuntimeState == ETCFBuildingRuntimeState::UnderConstruction
		&& !IsConstructionComplete();
}

float ATCFBuildingActor::GetConstructionWorkCompleted() const
{
	return ConstructionWorkCompleted;
}

float ATCFBuildingActor::GetRequiredConstructionWork() const
{
	return BuildingDefinition
		? BuildingDefinition->GetSafeRequiredConstructionWork()
		: 0.0f;
}

float ATCFBuildingActor::GetConstructionProgressAlpha() const
{
	const float RequiredWork = GetRequiredConstructionWork();
	if (RequiredWork <= 0.0f)
	{
		return IsConstructionComplete() ? 1.0f : 0.0f;
	}

	return FMath::Clamp(ConstructionWorkCompleted / RequiredWork, 0.0f, 1.0f);
}

bool ATCFBuildingActor::AddConstructionWork(float WorkAmount, AActor* WorkSource)
{
	if (!HasAuthority() || WorkAmount <= 0.0f || !CanReceiveConstructionWork())
	{
		return false;
	}

	SetConstructionWorkCompleted(ConstructionWorkCompleted + WorkAmount);

	if (IsConstructionComplete())
	{
		CompleteConstruction(WorkSource);
	}

	return true;
}

void ATCFBuildingActor::CompleteConstruction(AActor* CompletionSource)
{
	if (!HasAuthority() || RuntimeState != ETCFBuildingRuntimeState::UnderConstruction)
	{
		return;
	}

	const float RequiredWork = GetRequiredConstructionWork();
	if (RequiredWork > 0.0f)
	{
		SetConstructionWorkCompleted(RequiredWork);
	}

	SetRuntimeState(ETCFBuildingRuntimeState::Active);
	OnConstructionCompleted.Broadcast(this, CompletionSource);
}

UTCFAffiliationComponent* ATCFBuildingActor::GetAffiliationComponent() const
{
	return AffiliationComponent;
}

void ATCFBuildingActor::OnRep_BuildingDefinition()
{
	ApplyDefinitionVisuals();
	ApplyRuntimeStatePresentation();
}

void ATCFBuildingActor::OnRep_RuntimeState(ETCFBuildingRuntimeState OldRuntimeState)
{
	ApplyRuntimeStatePresentation();
	OnRuntimeStateChanged.Broadcast(OldRuntimeState, RuntimeState);
}

void ATCFBuildingActor::OnRep_ConstructionWorkCompleted(float OldConstructionWorkCompleted)
{
	OnConstructionProgressChanged.Broadcast(
		this,
		OldConstructionWorkCompleted,
		ConstructionWorkCompleted);
}

void ATCFBuildingActor::InitializeFromDefinition()
{
	ApplyDefinitionVisuals();
	
	if (!HasAuthority())
	{
		return;
	}

	if (BuildingDefinition && RuntimeState == ETCFBuildingRuntimeState::Inactive)
	{
		RuntimeState = BuildingDefinition->InitialRuntimeState;
	}

	InitializeConstructionStateFromDefinition();
	ApplyRuntimeStatePresentation();
}

void ATCFBuildingActor::ApplyDefinitionVisuals() const
{
	if (!BuildingDefinition)
	{
		return;
	}

	if (BuildingDefinition->BuildingMesh)
	{
		BuildingVisual->SetStaticMesh(BuildingDefinition->BuildingMesh);
	}

	if (BuildingDefinition->BuildingMaterial)
	{
		BuildingVisual->SetMaterial(0, BuildingDefinition->BuildingMaterial);
	}

	BuildingVisual->SetRelativeScale3D(BuildingDefinition->BuildingVisualScale);
	InteractionCollision->SetBoxExtent(BuildingDefinition->InteractionBoxExtent);
}

void ATCFBuildingActor::ApplyRuntimeStatePresentation()
{
	const bool bDestroyed = RuntimeState == ETCFBuildingRuntimeState::Destroyed;

	SetActorHiddenInGame(bDestroyed);
	SetActorEnableCollision(!bDestroyed);

	if (InteractionCollision)
	{
		InteractionCollision->SetCollisionEnabled(
			bDestroyed ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);
	}

	if (BuildingVisual)
	{
		BuildingVisual->SetVisibility(!bDestroyed, true);
	}
}

void ATCFBuildingActor::InitializeConstructionStateFromDefinition()
{
	if (!HasAuthority())
	{
		return;
	}

	const float RequiredWork = GetRequiredConstructionWork();

	if (RuntimeState == ETCFBuildingRuntimeState::UnderConstruction)
	{
		if (RequiredWork <= 0.0f)
		{
			CompleteConstruction(this);
			return;
		}

		ConstructionWorkCompleted = FMath::Clamp(
			ConstructionWorkCompleted,
			0.0f,
			RequiredWork);

		return;
	}

	if (RuntimeState == ETCFBuildingRuntimeState::Active)
	{
		ConstructionWorkCompleted = RequiredWork;
		return;
	}

	ConstructionWorkCompleted = 0.0f;
}

void ATCFBuildingActor::SetConstructionWorkCompleted(float NewConstructionWorkCompleted)
{
	if (!HasAuthority())
	{
		return;
	}

	const float RequiredWork = GetRequiredConstructionWork();
	const float OldProgress = ConstructionWorkCompleted;

	ConstructionWorkCompleted = RequiredWork > 0.0f
		? FMath::Clamp(NewConstructionWorkCompleted, 0.0f, RequiredWork)
		: 0.0f;

	if (!FMath::IsNearlyEqual(OldProgress, ConstructionWorkCompleted))
	{
		OnConstructionProgressChanged.Broadcast(
			this,
			OldProgress,
			ConstructionWorkCompleted);
	}
}

void ATCFBuildingActor::TryReservePlacementFootprint()
{
	if (!bReservePlacementGridOnBeginPlay || !BuildingDefinition || !BuildingDefinition->bBlocksBuildingPlacement)
	{
		return;
	}

	UWorld* World = GetWorld();
	UTCFRTSPlacementGridSubsystem* PlacementGridSubsystem = World
		? World->GetSubsystem<UTCFRTSPlacementGridSubsystem>()
		: nullptr;

	if (!PlacementGridSubsystem)
	{
		return;
	}

	FTCFPlacementGridValidationResult ValidationResult;
	FIntPoint AnchorCell = FIntPoint::ZeroValue;

	const bool bReserved = PlacementGridSubsystem->ReserveFootprintAtWorldLocation(
		this,
		GetActorLocation(),
		GetFootprintSize(),
		true,
		AnchorCell,
		ValidationResult);

	bHasReservedPlacementFootprint = bReserved;

	if (bReserved)
	{
		ReservedPlacementAnchorCell = AnchorCell;
	}
}

void ATCFBuildingActor::ReleasePlacementFootprint()
{
	UWorld* World = GetWorld();
	UTCFRTSPlacementGridSubsystem* PlacementGridSubsystem = World
		? World->GetSubsystem<UTCFRTSPlacementGridSubsystem>()
		: nullptr;

	if (PlacementGridSubsystem)
	{
		PlacementGridSubsystem->ReleaseFootprintForActor(this);
	}

	bHasReservedPlacementFootprint = false;
	ReservedPlacementAnchorCell = FIntPoint::ZeroValue;
}