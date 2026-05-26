//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFRTSHoverContextComponent.h"

#include "Actors/TCFCapturePointActor.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "GameFramework/PlayerController.h"
#include "Subsystems/TCFRelationshipSubsystem.h"

UTCFRTSHoverContextComponent::UTCFRTSHoverContextComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SetIsReplicatedByDefault(false);
}

void UTCFRTSHoverContextComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());

	SelectionComponent = GetOwner()
		? GetOwner()->FindComponentByClass<UTCFPlayerSelectionComponent>()
		: nullptr;

	RelationshipSubsystem = GetWorld()
		? GetWorld()->GetSubsystem<UTCFRelationshipSubsystem>()
		: nullptr;

	ForceRefreshHoverContext();
}

void UTCFRTSHoverContextComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RefreshHoverContext();
}

const FTCFRTSHoverContext& UTCFRTSHoverContextComponent::GetCurrentHoverContext() const
{
	return CurrentHoverContext;
}

void UTCFRTSHoverContextComponent::ForceRefreshHoverContext()
{
	RefreshHoverContext();
}

void UTCFRTSHoverContextComponent::RefreshHoverContext()
{
	FHitResult HitResult;
	FTCFRTSHoverContext NewHoverContext;

	if (TraceHover(HitResult))
	{
		NewHoverContext = BuildHoverContextFromHit(HitResult);
	}
	else
	{
		NewHoverContext.CursorState = ETCFRTSCursorState::Default;
		NewHoverContext.TargetType = ETCFRTSHoverTargetType::None;
	}

	if (!AreHoverContextsEquivalent(CurrentHoverContext, NewHoverContext))
	{
		CurrentHoverContext = NewHoverContext;
		OnHoverContextChanged.Broadcast(CurrentHoverContext);
	}
	else
	{
		CurrentHoverContext = NewHoverContext;
	}

	if (bApplyMouseCursor)
	{
		ApplyCursorState(CurrentHoverContext.CursorState);
	}
}

bool UTCFRTSHoverContextComponent::TraceHover(FHitResult& OutHitResult) const
{
	if (!PlayerController)
	{
		return false;
	}

	return PlayerController->GetHitResultUnderCursor(
		HoverTraceChannel,
		bTraceComplex,
		OutHitResult);
}

FTCFRTSHoverContext UTCFRTSHoverContextComponent::BuildHoverContextFromHit(const FHitResult& HitResult) const
{
	FTCFRTSHoverContext HoverContext;
	HoverContext.bHasHit = HitResult.bBlockingHit;
	HoverContext.HoveredActor = HitResult.GetActor();
	HoverContext.WorldLocation = HitResult.Location;
	HoverContext.WorldNormal = HitResult.ImpactNormal;
	HoverContext.bHasPrimarySelection = SelectionComponent && SelectionComponent->GetPrimarySelectedSquad();

	AActor* HitActor = HitResult.GetActor();
	HoverContext.TargetType = ResolveTargetType(HitActor);
	HoverContext.RelationshipToPrimarySelection = ResolveRelationshipToPrimarySelection(HitActor);

	const ATCFSquadActor* HoveredSquad = Cast<ATCFSquadActor>(HitActor);
	HoverContext.CursorState = ResolveCursorState(HoverContext, HoveredSquad);

	return HoverContext;
}

ETCFRTSHoverTargetType UTCFRTSHoverContextComponent::ResolveTargetType(const AActor* HitActor) const
{
	if (!HitActor)
	{
		return ETCFRTSHoverTargetType::Ground;
	}

	if (HitActor->IsA<ATCFSquadActor>())
	{
		return ETCFRTSHoverTargetType::Squad;
	}

	if (HitActor->IsA<ATCFCapturePointActor>())
	{
		return ETCFRTSHoverTargetType::CapturePoint;
	}

	if (ResourceNodeActorTag != NAME_None && HitActor->ActorHasTag(ResourceNodeActorTag))
	{
		return ETCFRTSHoverTargetType::ResourceNode;
	}

	if (ProductionBuildingActorTag != NAME_None && HitActor->ActorHasTag(ProductionBuildingActorTag))
	{
		return ETCFRTSHoverTargetType::ProductionBuilding;
	}

	return ETCFRTSHoverTargetType::Ground;
}

ETCFRTSCursorState UTCFRTSHoverContextComponent::ResolveCursorState(
	const FTCFRTSHoverContext& HoverContext,
	const ATCFSquadActor* HoveredSquad) const
{
	switch (HoverContext.TargetType)
	{
	case ETCFRTSHoverTargetType::Squad:
		if (!HoveredSquad)
		{
			return ETCFRTSCursorState::InvalidTarget;
		}

		if (!HoverContext.bHasPrimarySelection)
		{
			return ETCFRTSCursorState::SelectableOwn;
		}

		switch (HoverContext.RelationshipToPrimarySelection)
		{
		case ETCFSquadRelationship::Own:
			return ETCFRTSCursorState::SelectableOwn;

		case ETCFSquadRelationship::Friendly:
			return ETCFRTSCursorState::SelectableFriendly;

		case ETCFSquadRelationship::Enemy:
			return ETCFRTSCursorState::AttackEnemy;

		case ETCFSquadRelationship::Neutral:
		default:
			return ETCFRTSCursorState::InvalidTarget;
		}

	case ETCFRTSHoverTargetType::CapturePoint:
		return ETCFRTSCursorState::CapturePoint;

	case ETCFRTSHoverTargetType::ResourceNode:
		return ETCFRTSCursorState::ResourceNode;

	case ETCFRTSHoverTargetType::ProductionBuilding:
		return ETCFRTSCursorState::ProductionBuilding;

	case ETCFRTSHoverTargetType::Ground:
		return SelectionComponent && SelectionComponent->HasSelectedSquads()
			? ETCFRTSCursorState::MoveGround
			: ETCFRTSCursorState::Default;

	case ETCFRTSHoverTargetType::OtherActor:
		return ETCFRTSCursorState::InvalidTarget;

	case ETCFRTSHoverTargetType::None:
	default:
		return ETCFRTSCursorState::Default;
	}
}

ETCFSquadRelationship UTCFRTSHoverContextComponent::ResolveRelationshipToPrimarySelection(const AActor* HoveredActor) const
{
	if (!HoveredActor || !SelectionComponent || !RelationshipSubsystem)
	{
		return ETCFSquadRelationship::Neutral;
	}

	const ATCFSquadActor* PrimarySelectedSquad = SelectionComponent->GetPrimarySelectedSquad();
	if (!PrimarySelectedSquad)
	{
		return ETCFSquadRelationship::Neutral;
	}

	return RelationshipSubsystem->GetActorRelationship(PrimarySelectedSquad, HoveredActor);
}

void UTCFRTSHoverContextComponent::ApplyCursorState(ETCFRTSCursorState CursorState) const
{
	if (!PlayerController)
	{
		return;
	}

	PlayerController->CurrentMouseCursor = GetMouseCursorForState(CursorState);
}

EMouseCursor::Type UTCFRTSHoverContextComponent::GetMouseCursorForState(ETCFRTSCursorState CursorState) const
{
	switch (CursorState)
	{
	case ETCFRTSCursorState::MoveGround:
		return MoveGroundCursor.GetValue();

	case ETCFRTSCursorState::SelectableOwn:
		return SelectableOwnCursor.GetValue();

	case ETCFRTSCursorState::SelectableFriendly:
		return SelectableFriendlyCursor.GetValue();

	case ETCFRTSCursorState::AttackEnemy:
		return AttackEnemyCursor.GetValue();

	case ETCFRTSCursorState::CapturePoint:
		return CapturePointCursor.GetValue();

	case ETCFRTSCursorState::ResourceNode:
		return ResourceNodeCursor.GetValue();

	case ETCFRTSCursorState::ProductionBuilding:
		return ProductionBuildingCursor.GetValue();

	case ETCFRTSCursorState::InvalidTarget:
		return InvalidTargetCursor.GetValue();

	case ETCFRTSCursorState::Default:
	default:
		return DefaultCursor.GetValue();
	}
}

bool UTCFRTSHoverContextComponent::AreHoverContextsEquivalent(
	const FTCFRTSHoverContext& A,
	const FTCFRTSHoverContext& B) const
{
	return A.bHasHit == B.bHasHit
		&& A.TargetType == B.TargetType
		&& A.CursorState == B.CursorState
		&& A.HoveredActor == B.HoveredActor
		&& A.RelationshipToPrimarySelection == B.RelationshipToPrimarySelection
		&& A.bHasPrimarySelection == B.bHasPrimarySelection;
}