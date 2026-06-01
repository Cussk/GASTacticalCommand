//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Actors/TCFBuildingActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TCFAffiliationComponent.h"
#include "Net/UnrealNetwork.h"

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
}

void ATCFBuildingActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATCFBuildingActor, BuildingDefinition);
	DOREPLIFETIME(ATCFBuildingActor, RuntimeState);
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

void ATCFBuildingActor::InitializeFromDefinition()
{
	ApplyDefinitionVisuals();

	if (HasAuthority() && BuildingDefinition && RuntimeState == ETCFBuildingRuntimeState::Inactive)
	{
		RuntimeState = BuildingDefinition->InitialRuntimeState;
	}

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