//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFRTSOrderTargetingComponent.h"

#include "Actors/TCFSquadActor.h"
#include "Components/DecalComponent.h"
#include "Components/TCFPlayerOrderSubmissionComponent.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Components/TCFRTSHoverContextComponent.h"
#include "Data/TCFOrderDefinition.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"

UTCFRTSOrderTargetingComponent::UTCFRTSOrderTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(false);
}

void UTCFRTSOrderTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* OwnerActor = GetOwner();
	PlayerController = Cast<APlayerController>(OwnerActor);

	if (OwnerActor)
	{
		SelectionComponent = OwnerActor->FindComponentByClass<UTCFPlayerSelectionComponent>();
		PlayerOrderComponent = OwnerActor->FindComponentByClass<UTCFPlayerOrderSubmissionComponent>();
		HoverContextComponent = OwnerActor->FindComponentByClass<UTCFRTSHoverContextComponent>();
	}
}

void UTCFRTSOrderTargetingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyPreview();

	Super::EndPlay(EndPlayReason);
}

bool UTCFRTSOrderTargetingComponent::BeginOrderTargeting(UTCFOrderDefinition* OrderDefinition)
{
	if (!IsValid(OrderDefinition) || !OrderDefinition->OrderTag.IsValid())
	{
		return false;
	}
	
	if (!SelectionComponent || !SelectionComponent->HasSelectedSquads())
	{
		CancelOrderTargeting();
		return false;
	}

	if (!PlayerOrderComponent)
	{
		return false;
	}

	if (OrderDefinition->Targeting.TargetType == ETCFOrderTargetType::None
		|| OrderDefinition->Targeting.TargetType == ETCFOrderTargetType::Self)
	{
		return SubmitSelfOrder(*OrderDefinition);
	}

	PendingOrderDefinition = OrderDefinition;
	bCurrentTargetValid = false;

	SetComponentTickEnabled(true);
	RefreshTargetingPreview();

	OnOrderTargetingStarted.Broadcast(PendingOrderDefinition);
	return true;
}

void UTCFRTSOrderTargetingComponent::CancelOrderTargeting()
{
	if (!PendingOrderDefinition)
	{
		return;
	}

	PendingOrderDefinition = nullptr;
	bCurrentTargetValid = false;

	if (HoverContextComponent)
	{
		HoverContextComponent->ClearCursorOverride();
	}

	HidePreview();
	SetComponentTickEnabled(false);

	OnOrderTargetingCanceled.Broadcast();
}

bool UTCFRTSOrderTargetingComponent::ConfirmPendingOrder()
{
	if (!PendingOrderDefinition || !PlayerOrderComponent)
	{
		return false;
	}

	RefreshTargetingPreview();

	if (!bCurrentTargetValid)
	{
		return false;
	}

	FTCFOrderTarget Target;
	if (!BuildTargetFromHover(Target))
	{
		return false;
	}

	TArray<FTCFOrderResult> Results;
	const bool bSubmitted = PlayerOrderComponent->SubmitSelectedSquadsOrder(
		PendingOrderDefinition->OrderTag,
		Target,
		Results);

	OnOrderTargetingConfirmed.Broadcast(Target);

	CancelOrderTargeting();
	return bSubmitted;
}

bool UTCFRTSOrderTargetingComponent::HasPendingOrder() const
{
	return PendingOrderDefinition != nullptr;
}

UTCFOrderDefinition* UTCFRTSOrderTargetingComponent::GetPendingOrderDefinition() const
{
	return PendingOrderDefinition;
}

bool UTCFRTSOrderTargetingComponent::IsCurrentTargetValid() const
{
	return bCurrentTargetValid;
}

void UTCFRTSOrderTargetingComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RefreshTargetingPreview();
}

void UTCFRTSOrderTargetingComponent::RefreshTargetingPreview()
{
	if (!SelectionComponent || !SelectionComponent->HasSelectedSquads())
	{
		CancelOrderTargeting();
		return;
	}

	bCurrentTargetValid = IsHoverValidForPendingOrder();

	RefreshCursorOverride();
	RefreshPreview();
}

void UTCFRTSOrderTargetingComponent::RefreshCursorOverride() const
{
	if (!HoverContextComponent || !PendingOrderDefinition)
	{
		return;
	}

	HoverContextComponent->SetCursorOverride(
		bCurrentTargetValid
			? ETCFRTSCursorState::OrderTarget
			: ETCFRTSCursorState::OrderTargetInvalid);
}

void UTCFRTSOrderTargetingComponent::RefreshPreview()
{
	if (!PendingOrderDefinition || !PendingOrderDefinition->TargetingPreview.bShowPreview)
	{
		HidePreview();
		return;
	}

	const ETCFOrderTargetType TargetType = PendingOrderDefinition->Targeting.TargetType;
	const bool bCanUseGroundPreview =
		TargetType == ETCFOrderTargetType::Location
		|| TargetType == ETCFOrderTargetType::Area
		|| TargetType == ETCFOrderTargetType::Direction;

	if (!bCanUseGroundPreview)
	{
		HidePreview();
		return;
	}

	if (!bCurrentTargetValid && !bShowInvalidLocationPreview)
	{
		HidePreview();
		return;
	}

	CreateOrUpdatePreview();
}

bool UTCFRTSOrderTargetingComponent::SubmitSelfOrder(const UTCFOrderDefinition& OrderDefinition) const
{
	FTCFOrderTarget Target;
	Target.TargetType = ETCFOrderTargetType::Self;

	TArray<FTCFOrderResult> Results;
	return PlayerOrderComponent->SubmitSelectedSquadsOrder(
		OrderDefinition.OrderTag,
		Target,
		Results);
}

bool UTCFRTSOrderTargetingComponent::BuildTargetFromHover(FTCFOrderTarget& OutTarget) const
{
	if (!PendingOrderDefinition || !HoverContextComponent)
	{
		return false;
	}

	const FTCFRTSHoverContext& HoverContext = HoverContextComponent->GetCurrentHoverContext();

	OutTarget = FTCFOrderTarget();
	OutTarget.TargetType = PendingOrderDefinition->Targeting.TargetType;
	OutTarget.Radius = PendingOrderDefinition->Targeting.Radius;
	OutTarget.ConeAngleDegrees = PendingOrderDefinition->Targeting.ConeAngleDegrees;
	
	FHitResult GroundHit;

	switch (PendingOrderDefinition->Targeting.TargetType)
	{
	case ETCFOrderTargetType::Actor:
		if (!IsValid(HoverContext.HoveredActor))
		{
			return false;
		}

		OutTarget.TargetActor = HoverContext.HoveredActor;
		OutTarget.TargetLocation = HoverContext.HoveredActor->GetActorLocation();
		return true;

	case ETCFOrderTargetType::Location:
	case ETCFOrderTargetType::Area:
		if (!TraceGroundPreview(GroundHit))
		{
			return false;
		}

		OutTarget.TargetLocation = GroundHit.ImpactPoint;
		return true;

	case ETCFOrderTargetType::Direction:
		if (!TraceGroundPreview(GroundHit) || !SelectionComponent)
		{
			return false;
		}

		if (const AActor* PrimarySelectedActor = SelectionComponent->GetPrimarySelectedSquad())
		{
			OutTarget.TargetLocation = GroundHit.ImpactPoint;
			OutTarget.TargetDirection = (GroundHit.ImpactPoint - PrimarySelectedActor->GetActorLocation()).GetSafeNormal();
			return !OutTarget.TargetDirection.IsNearlyZero();
		}

		return false;

	case ETCFOrderTargetType::Self:
		OutTarget.TargetType = ETCFOrderTargetType::Self;
		return true;

	case ETCFOrderTargetType::None:
	default:
		return false;
	}
}

bool UTCFRTSOrderTargetingComponent::IsHoverValidForPendingOrder() const
{
	if (!PendingOrderDefinition || !HoverContextComponent)
	{
		return false;
	}

	switch (PendingOrderDefinition->Targeting.TargetType)
	{
	case ETCFOrderTargetType::Actor:
		return IsActorTargetValid();

	case ETCFOrderTargetType::Location:
		return IsLocationTargetValid();

	case ETCFOrderTargetType::Direction:
		return IsDirectionTargetValid();

	case ETCFOrderTargetType::Area:
		return IsAreaTargetValid();

	case ETCFOrderTargetType::Self:
		return true;

	case ETCFOrderTargetType::None:
	default:
		return false;
	}
}

bool UTCFRTSOrderTargetingComponent::IsActorTargetValid() const
{
	const FTCFRTSHoverContext& HoverContext = HoverContextComponent->GetCurrentHoverContext();

	if (!IsValid(HoverContext.HoveredActor))
	{
		return false;
	}

	if (!PendingOrderDefinition->Targeting.bUseRelationshipFilter)
	{
		return true;
	}

	return PendingOrderDefinition->Targeting.RelationshipFilter.AllowsRelationship(
		HoverContext.RelationshipToPrimarySelection);
}

bool UTCFRTSOrderTargetingComponent::IsLocationTargetValid() const
{
	FHitResult GroundHit;
	return TraceGroundPreview(GroundHit);
}

bool UTCFRTSOrderTargetingComponent::IsAreaTargetValid() const
{
	FHitResult GroundHit;
	return TraceGroundPreview(GroundHit);
}

bool UTCFRTSOrderTargetingComponent::IsDirectionTargetValid() const
{
	FHitResult GroundHit;
	return TraceGroundPreview(GroundHit)
		&& SelectionComponent
		&& IsValid(SelectionComponent->GetPrimarySelectedSquad());
}

void UTCFRTSOrderTargetingComponent::CreateOrUpdatePreview()
{
	if (!PendingOrderDefinition || !HoverContextComponent)
	{
		return;
	}

	FHitResult GroundHit;
	if (!TraceGroundPreview(GroundHit))
	{
		HidePreview();
		return;
	}

	UStaticMesh* PreviewMesh = ResolvePreviewMesh();
	UMaterialInterface* PreviewMaterial = ResolvePreviewMaterial();

	if (!PreviewMesh || !PreviewMaterial)
	{
		HidePreview();
		return;
	}

	const FVector PreviewNormal = GroundHit.ImpactNormal.IsNearlyZero()
	? FVector::UpVector
	: GroundHit.ImpactNormal.GetSafeNormal();

	const float GroundOffset = PendingOrderDefinition
		? PendingOrderDefinition->TargetingPreview.GroundOffset
		: 4.0f;

	const FVector PreviewLocation = GroundHit.ImpactPoint + PreviewNormal * GroundOffset;
	const FRotator PreviewRotation = GetPreviewRotationForPendingOrder(PreviewLocation);
	const FVector PreviewScale = GetPreviewScaleForPendingOrder();

	if (!TargetingPreviewActor)
	{
		UWorld* World = GetWorld();
		if (!World)
		{
			return;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.ObjectFlags |= RF_Transient;

		TargetingPreviewActor = World->SpawnActor<AStaticMeshActor>(
			AStaticMeshActor::StaticClass(),
			PreviewLocation,
			PreviewRotation,
			SpawnParams);

		if (!TargetingPreviewActor)
		{
			return;
		}

		TargetingPreviewActor->SetActorEnableCollision(false);
		TargetingPreviewActor->SetActorHiddenInGame(false);
	}

	UStaticMeshComponent* PreviewMeshComponent = TargetingPreviewActor->GetStaticMeshComponent();
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
	PreviewMeshComponent->SetMaterial(0, PreviewMaterial);

	TargetingPreviewActor->SetActorLocation(PreviewLocation);
	TargetingPreviewActor->SetActorRotation(PreviewRotation);
	TargetingPreviewActor->SetActorScale3D(PreviewScale);
	TargetingPreviewActor->SetActorHiddenInGame(false);
}

void UTCFRTSOrderTargetingComponent::HidePreview() const
{
	if (TargetingPreviewActor)
	{
		TargetingPreviewActor->SetActorHiddenInGame(true);
	}
}

void UTCFRTSOrderTargetingComponent::DestroyPreview()
{
	if (TargetingPreviewActor)
	{
		TargetingPreviewActor->Destroy();
		TargetingPreviewActor = nullptr;
	}
}

FVector UTCFRTSOrderTargetingComponent::GetPreviewScaleForPendingOrder() const
{
	if (!PendingOrderDefinition)
	{
		return FVector(1.0f);
	}

	const FTCFOrderTargetingPreviewConfig& PreviewConfig = PendingOrderDefinition->TargetingPreview;

	if (PreviewConfig.bScaleToOrderRadius)
	{
		const float Radius = FMath::Max(
			PreviewConfig.MinimumRadius,
			PendingOrderDefinition->Targeting.Radius);

		// Assumes a default UE plane mesh that is 100x100 units.
		const float Diameter = Radius * 2.0f;
		constexpr float PlaneBaseSize = 100.0f;
		const float UniformScale = Diameter / PlaneBaseSize;

		return FVector(UniformScale, UniformScale, 1.0f);
	}

	// Also assumes a default 100x100 plane.
	constexpr float PlaneBaseSize = 100.0f;
	return FVector(
		PreviewConfig.PreviewSize.Y / PlaneBaseSize,
		PreviewConfig.PreviewSize.Z / PlaneBaseSize,
		1.0f);
}

UStaticMesh* UTCFRTSOrderTargetingComponent::ResolvePreviewMesh() const
{
	if (PendingOrderDefinition && !PendingOrderDefinition->TargetingPreview.PreviewMesh.IsNull())
	{
		if (UStaticMesh* OrderPreviewMesh = PendingOrderDefinition->TargetingPreview.PreviewMesh.LoadSynchronous())
		{
			return OrderPreviewMesh;
		}
	}

	if (!DefaultPreviewMesh.IsNull())
	{
		if (UStaticMesh* DefaultMesh = DefaultPreviewMesh.LoadSynchronous())
		{
			return DefaultMesh;
		}
	}

	return LoadObject<UStaticMesh>(
		nullptr,
		TEXT("/Engine/BasicShapes/Plane.Plane"));
}

UMaterialInterface* UTCFRTSOrderTargetingComponent::ResolvePreviewMaterial() const
{
	if (PendingOrderDefinition && !PendingOrderDefinition->TargetingPreview.PreviewMaterial.IsNull())
	{
		return PendingOrderDefinition->TargetingPreview.PreviewMaterial.LoadSynchronous();
	}

	return DefaultPreviewMaterial.LoadSynchronous();
}

FRotator UTCFRTSOrderTargetingComponent::GetPreviewRotationForPendingOrder(const FVector& PreviewLocation) const
{
	if (!PendingOrderDefinition)
	{
		return FRotator::ZeroRotator;
	}

	if (PendingOrderDefinition->Targeting.TargetType != ETCFOrderTargetType::Direction)
	{
		return FRotator::ZeroRotator;
	}

	if (!SelectionComponent)
	{
		return FRotator::ZeroRotator;
	}

	const AActor* PrimarySelectedActor = SelectionComponent->GetPrimarySelectedSquad();
	if (!PrimarySelectedActor)
	{
		return FRotator::ZeroRotator;
	}

	FVector Direction = PreviewLocation - PrimarySelectedActor->GetActorLocation();
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero())
	{
		return FRotator::ZeroRotator;
	}

	return Direction.Rotation();
}

bool UTCFRTSOrderTargetingComponent::TraceGroundPreview(FHitResult& OutHitResult) const
{
	if (!PlayerController)
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
	const FVector TraceEnd = TraceStart + WorldDirection * GroundPreviewTraceDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TCFOrderTargetingGroundPreviewTrace), false);
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.AddIgnoredActor(PlayerController);
	QueryParams.AddIgnoredActor(GetOwner());

	return GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		TraceStart,
		TraceEnd,
		GroundPreviewTraceChannel,
		QueryParams);
}
