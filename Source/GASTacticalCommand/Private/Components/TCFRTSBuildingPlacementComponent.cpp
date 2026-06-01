//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFRTSBuildingPlacementComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TCFRTSHoverContextComponent.h"
#include "Components/TCFRTSSelectionBoxComponent.h"
#include "Data/TCFBuildingDefinition.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInterface.h"
#include "Subsystems/TCFRTSPlacementGridSubsystem.h"
#include "Types/TCFRTSControlTypes.h"

UTCFRTSBuildingPlacementComponent::UTCFRTSBuildingPlacementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(false);
}

void UTCFRTSBuildingPlacementComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());

	if (AActor* OwnerActor = GetOwner())
	{
		HoverContextComponent = OwnerActor->FindComponentByClass<UTCFRTSHoverContextComponent>();
		SelectionBoxComponent = OwnerActor->FindComponentByClass<UTCFRTSSelectionBoxComponent>();
	}
}

void UTCFRTSBuildingPlacementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyPreview();

	Super::EndPlay(EndPlayReason);
}

bool UTCFRTSBuildingPlacementComponent::BeginBuildingPlacement(UTCFBuildingDefinition* BuildingDefinition)
{
	if (!IsValid(BuildingDefinition))
	{
		return false;
	}

	PendingBuildingDefinition = BuildingDefinition;
	bCurrentPlacementValid = false;
	CurrentValidationResult = FTCFPlacementGridValidationResult();
	
	if (SelectionBoxComponent)
	{
		SelectionBoxComponent->CancelSelection();
	}

	SetComponentTickEnabled(true);
	RefreshPlacement();

	OnBuildingPlacementStarted.Broadcast(PendingBuildingDefinition);
	return true;
}

void UTCFRTSBuildingPlacementComponent::CancelBuildingPlacement()
{
	if (!PendingBuildingDefinition)
	{
		return;
	}

	PendingBuildingDefinition = nullptr;
	bCurrentPlacementValid = false;
	CurrentValidationResult = FTCFPlacementGridValidationResult();

	if (HoverContextComponent)
	{
		HoverContextComponent->ClearCursorOverride();
	}

	HidePreview();
	SetComponentTickEnabled(false);

	OnBuildingPlacementCanceled.Broadcast();
}

bool UTCFRTSBuildingPlacementComponent::ConfirmBuildingPlacement()
{
	if (!PendingBuildingDefinition)
	{
		return false;
	}

	RefreshPlacement();

	if (!bCurrentPlacementValid)
	{
		return false;
	}

	OnBuildingPlacementConfirmed.Broadcast(
		PendingBuildingDefinition,
		CurrentPlacementLocation,
		CurrentAnchorCell,
		CurrentValidationResult);

	// V2.6.4 will spend resources, spawn the building, and reserve the final footprint.
	CancelBuildingPlacement();
	return true;
}

bool UTCFRTSBuildingPlacementComponent::IsPlacingBuilding() const
{
	return PendingBuildingDefinition != nullptr;
}

bool UTCFRTSBuildingPlacementComponent::IsCurrentPlacementValid() const
{
	return bCurrentPlacementValid;
}

UTCFBuildingDefinition* UTCFRTSBuildingPlacementComponent::GetPendingBuildingDefinition() const
{
	return PendingBuildingDefinition;
}

FVector UTCFRTSBuildingPlacementComponent::GetCurrentPlacementLocation() const
{
	return CurrentPlacementLocation;
}

FIntPoint UTCFRTSBuildingPlacementComponent::GetCurrentAnchorCell() const
{
	return CurrentAnchorCell;
}

FTCFPlacementGridValidationResult UTCFRTSBuildingPlacementComponent::GetCurrentValidationResult() const
{
	return CurrentValidationResult;
}

void UTCFRTSBuildingPlacementComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RefreshPlacement();
}

void UTCFRTSBuildingPlacementComponent::RefreshPlacement()
{
	if (!PendingBuildingDefinition)
	{
		return;
	}

	bCurrentPlacementValid = UpdatePlacementFromTrace();

	RefreshCursorOverride();
	RefreshPreview();
}

void UTCFRTSBuildingPlacementComponent::RefreshCursorOverride() const
{
	if (!HoverContextComponent || !PendingBuildingDefinition)
	{
		return;
	}

	HoverContextComponent->SetCursorOverride(
		bCurrentPlacementValid
			? ETCFRTSCursorState::BuildingPlacement
			: ETCFRTSCursorState::BuildingPlacementInvalid);
}

void UTCFRTSBuildingPlacementComponent::RefreshPreview()
{
	if (!PendingBuildingDefinition)
	{
		HidePreview();
		return;
	}

	if (!bCurrentPlacementValid && !bShowInvalidPreview)
	{
		HidePreview();
		return;
	}

	UStaticMesh* PreviewMesh = ResolvePreviewMesh();
	if (!PreviewMesh)
	{
		HidePreview();
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		HidePreview();
		return;
	}

	const FVector PreviewLocation = CurrentPlacementLocation + CurrentPlacementNormal * PreviewGroundOffset;

	if (!PreviewActor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.ObjectFlags |= RF_Transient;

		PreviewActor = World->SpawnActor<AStaticMeshActor>(
			AStaticMeshActor::StaticClass(),
			PreviewLocation,
			FRotator::ZeroRotator,
			SpawnParams);

		if (!PreviewActor)
		{
			return;
		}

		PreviewActor->SetActorEnableCollision(false);
		PreviewActor->SetActorHiddenInGame(false);
	}

	UStaticMeshComponent* PreviewMeshComponent = PreviewActor->GetStaticMeshComponent();
	if (!PreviewMeshComponent)
	{
		HidePreview();
		return;
	}

	PreviewMeshComponent->SetMobility(EComponentMobility::Movable);
	PreviewMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewMeshComponent->SetGenerateOverlapEvents(false);
	PreviewMeshComponent->SetCastShadow(false);
	PreviewMeshComponent->bReceivesDecals = false;
	PreviewMeshComponent->SetVisibility(true);
	PreviewMeshComponent->SetHiddenInGame(false);
	PreviewMeshComponent->SetStaticMesh(PreviewMesh);

	if (UMaterialInterface* PreviewMaterial = ResolvePreviewMaterial())
	{
		PreviewMeshComponent->SetMaterial(0, PreviewMaterial);
	}

	PreviewActor->SetActorLocation(PreviewLocation);
	PreviewActor->SetActorRotation(FRotator::ZeroRotator);
	PreviewActor->SetActorScale3D(ResolvePreviewScale());
	PreviewActor->SetActorHiddenInGame(false);
}

void UTCFRTSBuildingPlacementComponent::HidePreview() const
{
	if (PreviewActor)
	{
		PreviewActor->SetActorHiddenInGame(true);
	}
}

void UTCFRTSBuildingPlacementComponent::DestroyPreview()
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}
}

bool UTCFRTSBuildingPlacementComponent::TraceGround(FHitResult& OutHitResult) const
{
	if (!PlayerController || !GetWorld())
	{
		return false;
	}

	FVector WorldOrigin;
	FVector WorldDirection;
	if (!PlayerController->DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
	{
		return false;
	}

	const FVector TraceStart = WorldOrigin;
	const FVector TraceEnd = TraceStart + WorldDirection * GroundTraceDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TCFBuildingPlacementGroundTrace), false);
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.AddIgnoredActor(PlayerController);
	QueryParams.AddIgnoredActor(GetOwner());

	if (PreviewActor)
	{
		QueryParams.AddIgnoredActor(PreviewActor);
	}

	return GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		TraceStart,
		TraceEnd,
		GroundTraceChannel,
		QueryParams);
}

bool UTCFRTSBuildingPlacementComponent::UpdatePlacementFromTrace()
{
	if (!PendingBuildingDefinition)
	{
		return false;
	}

	FHitResult GroundHit;
	if (!TraceGround(GroundHit))
	{
		CurrentValidationResult = FTCFPlacementGridValidationResult::Failure(
			ETCFPlacementGridValidationFailure::InvalidFootprint);

		return false;
	}

	CurrentPlacementNormal = GroundHit.ImpactNormal.IsNearlyZero()
		? FVector::UpVector
		: GroundHit.ImpactNormal.GetSafeNormal();

	UTCFRTSPlacementGridSubsystem* PlacementGridSubsystem = GetPlacementGridSubsystem();
	if (!PlacementGridSubsystem)
	{
		CurrentPlacementLocation = GroundHit.ImpactPoint;
		CurrentAnchorCell = FIntPoint::ZeroValue;
		CurrentValidationResult = FTCFPlacementGridValidationResult::Failure(
			ETCFPlacementGridValidationFailure::InvalidFootprint);

		return false;
	}

	const FIntPoint FootprintSize = PendingBuildingDefinition->GetSafeFootprintSize();

	const bool bValid = PlacementGridSubsystem->ValidateFootprintAtWorldLocation(
		GroundHit.ImpactPoint,
		FootprintSize,
		nullptr,
		CurrentAnchorCell,
		CurrentValidationResult);

	CurrentPlacementLocation = PlacementGridSubsystem->GetFootprintCenterWorldLocation(
		CurrentAnchorCell,
		FootprintSize);

	CurrentPlacementLocation.Z = GroundHit.ImpactPoint.Z;

	return bValid;
}

UStaticMesh* UTCFRTSBuildingPlacementComponent::ResolvePreviewMesh() const
{
	if (!PendingBuildingDefinition)
	{
		return nullptr;
	}

	if (PendingBuildingDefinition->PreviewMesh)
	{
		return PendingBuildingDefinition->PreviewMesh;
	}

	if (PendingBuildingDefinition->BuildingMesh)
	{
		return PendingBuildingDefinition->BuildingMesh;
	}

	return LoadObject<UStaticMesh>(
		nullptr,
		TEXT("/Engine/BasicShapes/Cube.Cube"));
}

UMaterialInterface* UTCFRTSBuildingPlacementComponent::ResolvePreviewMaterial() const
{
	if (!PendingBuildingDefinition)
	{
		return nullptr;
	}

	if (bCurrentPlacementValid)
	{
		return PendingBuildingDefinition->ValidPreviewMaterial
			? PendingBuildingDefinition->ValidPreviewMaterial
			: DefaultValidPreviewMaterial;
	}

	if (PendingBuildingDefinition->InvalidPreviewMaterial)
	{
		return PendingBuildingDefinition->InvalidPreviewMaterial;
	}

	return DefaultInvalidPreviewMaterial
		? DefaultInvalidPreviewMaterial
		: DefaultValidPreviewMaterial;
}

FVector UTCFRTSBuildingPlacementComponent::ResolvePreviewScale() const
{
	if (!PendingBuildingDefinition)
	{
		return FVector(1.0f);
	}

	if (!PendingBuildingDefinition->PreviewVisualScale.Equals(FVector(1.0f)))
	{
		return PendingBuildingDefinition->PreviewVisualScale;
	}

	return PendingBuildingDefinition->BuildingVisualScale;
}

UTCFRTSPlacementGridSubsystem* UTCFRTSBuildingPlacementComponent::GetPlacementGridSubsystem() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UTCFRTSPlacementGridSubsystem>() : nullptr;
}