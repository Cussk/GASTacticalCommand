//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFRTSOrderTargetingComponent.h"

#include "Actors/TCFSquadActor.h"
#include "Components/DecalComponent.h"
#include "Components/TCFPlayerOrderComponent.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Components/TCFRTSHoverContextComponent.h"
#include "Data/TCFOrderDefinition.h"
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
		PlayerOrderComponent = OwnerActor->FindComponentByClass<UTCFPlayerOrderComponent>();
		HoverContextComponent = OwnerActor->FindComponentByClass<UTCFRTSHoverContextComponent>();
	}
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

	HideDecal();
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
	RefreshDecal();
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

void UTCFRTSOrderTargetingComponent::RefreshDecal()
{
	if (!PendingOrderDefinition || !PendingOrderDefinition->TargetingDecal.bShowDecal)
	{
		HideDecal();
		return;
	}

	const ETCFOrderTargetType TargetType = PendingOrderDefinition->Targeting.TargetType;
	const bool bCanUseGroundDecal =
		TargetType == ETCFOrderTargetType::Location
		|| TargetType == ETCFOrderTargetType::Area
		|| TargetType == ETCFOrderTargetType::Direction;

	if (!bCanUseGroundDecal)
	{
		HideDecal();
		return;
	}

	if (!bCurrentTargetValid && !bShowInvalidLocationDecal)
	{
		HideDecal();
		return;
	}

	CreateOrUpdateDecal();
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
		if (!HoverContext.bHasHit)
		{
			return false;
		}

		OutTarget.TargetLocation = HoverContext.WorldLocation;
		return true;

	case ETCFOrderTargetType::Direction:
		if (!HoverContext.bHasHit || !SelectionComponent)
		{
			return false;
		}

		if (const AActor* PrimarySelectedActor = SelectionComponent->GetPrimarySelectedSquad())
		{
			OutTarget.TargetLocation = HoverContext.WorldLocation;
			OutTarget.TargetDirection = (HoverContext.WorldLocation - PrimarySelectedActor->GetActorLocation()).GetSafeNormal();
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
	const FTCFRTSHoverContext& HoverContext = HoverContextComponent->GetCurrentHoverContext();
	return HoverContext.bHasHit;
}

bool UTCFRTSOrderTargetingComponent::IsDirectionTargetValid() const
{
	const FTCFRTSHoverContext& HoverContext = HoverContextComponent->GetCurrentHoverContext();
	return HoverContext.bHasHit
		&& SelectionComponent
		&& IsValid(SelectionComponent->GetPrimarySelectedSquad());
}

bool UTCFRTSOrderTargetingComponent::IsAreaTargetValid() const
{
	const FTCFRTSHoverContext& HoverContext = HoverContextComponent->GetCurrentHoverContext();
	return HoverContext.bHasHit;
}

void UTCFRTSOrderTargetingComponent::CreateOrUpdateDecal()
{
	if (!PendingOrderDefinition || !HoverContextComponent)
	{
		return;
	}

	const FTCFRTSHoverContext& HoverContext = HoverContextComponent->GetCurrentHoverContext();
	if (!HoverContext.bHasHit)
	{
		HideDecal();
		return;
	}

	UMaterialInterface* DecalMaterial = ResolveDecalMaterial();
	if (!DecalMaterial)
	{
		HideDecal();
		return;
	}

	const FVector DecalNormal = HoverContext.WorldNormal.IsNearlyZero()
		? FVector::UpVector
		: HoverContext.WorldNormal.GetSafeNormal();

	const FVector DecalLocation = HoverContext.WorldLocation + DecalNormal * 4.0f;
	const FVector DecalSize = GetDecalSizeForPendingOrder();
	const FRotator DecalRotation = FRotator(-90.0f, 0.0f, 0.0f);

	if (!TargetingDecalComponent)
	{
		TargetingDecalComponent = UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			DecalMaterial,
			DecalSize,
			DecalLocation,
			DecalRotation,
			0.0f);

		if (!TargetingDecalComponent)
		{
			return;
		}

		TargetingDecalComponent->SetFadeScreenSize(0.001f);
	}

	TargetingDecalComponent->SetDecalMaterial(DecalMaterial);
	TargetingDecalComponent->DecalSize = DecalSize;
	TargetingDecalComponent->SetWorldLocation(DecalLocation);
	TargetingDecalComponent->SetWorldRotation(DecalRotation);
	TargetingDecalComponent->SetHiddenInGame(false);
	TargetingDecalComponent->SetVisibility(true);
}

void UTCFRTSOrderTargetingComponent::HideDecal() const
{
	if (TargetingDecalComponent)
	{
		TargetingDecalComponent->SetVisibility(false);
	}
}

void UTCFRTSOrderTargetingComponent::DestroyDecal()
{
	if (TargetingDecalComponent)
	{
		TargetingDecalComponent->DestroyComponent();
		TargetingDecalComponent = nullptr;
	}
}

FVector UTCFRTSOrderTargetingComponent::GetDecalSizeForPendingOrder() const
{
	if (!PendingOrderDefinition)
	{
		return FVector(48.0f, 256.0f, 256.0f);
	}

	const FTCFOrderTargetingDecalConfig& DecalConfig = PendingOrderDefinition->TargetingDecal;

	if (DecalConfig.bScaleToOrderRadius)
	{
		const float Radius = FMath::Max(
			DecalConfig.MinimumRadius,
			PendingOrderDefinition->Targeting.Radius);

		return FVector(
			DecalConfig.DecalSize.X,
			Radius * 2.0f,
			Radius * 2.0f);
	}

	return DecalConfig.DecalSize;
}

UMaterialInterface* UTCFRTSOrderTargetingComponent::ResolveDecalMaterial() const
{
	if (!PendingOrderDefinition)
	{
		return DefaultDecalMaterial.LoadSynchronous();
	}

	if (!PendingOrderDefinition->TargetingDecal.DecalMaterial.IsNull())
	{
		return PendingOrderDefinition->TargetingDecal.DecalMaterial.LoadSynchronous();
	}

	return DefaultDecalMaterial.LoadSynchronous();
}