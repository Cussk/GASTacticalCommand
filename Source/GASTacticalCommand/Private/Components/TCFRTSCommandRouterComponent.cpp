//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFRTSCommandRouterComponent.h"

#include "Components/TCFPlayerMovementCommandComponent.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Components/TCFRTSHoverContextComponent.h"

UTCFRTSCommandRouterComponent::UTCFRTSCommandRouterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFRTSCommandRouterComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	SelectionComponent = OwnerActor->FindComponentByClass<UTCFPlayerSelectionComponent>();
	MovementCommandComponent = OwnerActor->FindComponentByClass<UTCFPlayerMovementCommandComponent>();
	HoverContextComponent = OwnerActor->FindComponentByClass<UTCFRTSHoverContextComponent>();
}

bool UTCFRTSCommandRouterComponent::ExecutePrimaryCommand()
{
	FTCFRTSCommandIntent CommandIntent = BuildCommandIntent();
	CommandIntent.bWasExecuted = ExecuteCommandIntent(CommandIntent);

	LastCommandIntent = CommandIntent;
	OnCommandIntentResolved.Broadcast(LastCommandIntent);

	return LastCommandIntent.bWasExecuted;
}

const FTCFRTSCommandIntent& UTCFRTSCommandRouterComponent::GetLastCommandIntent() const
{
	return LastCommandIntent;
}

FTCFRTSCommandIntent UTCFRTSCommandRouterComponent::BuildCommandIntent() const
{
	FTCFRTSCommandIntent CommandIntent;

	if (!HoverContextComponent)
	{
		CommandIntent.IntentType = ETCFRTSCommandIntentType::Invalid;
		return CommandIntent;
	}

	const FTCFRTSHoverContext& HoverContext = HoverContextComponent->GetCurrentHoverContext();

	CommandIntent.IntentType = ResolveIntentType(HoverContext);
	CommandIntent.TargetType = HoverContext.TargetType;
	CommandIntent.CursorState = HoverContext.CursorState;
	CommandIntent.TargetActor = HoverContext.HoveredActor;
	CommandIntent.TargetLocation = ResolveTargetLocation(HoverContext);
	CommandIntent.RelationshipToPrimarySelection = HoverContext.RelationshipToPrimarySelection;
	CommandIntent.SelectedSquadCount = GetSelectedSquadCount();

	return CommandIntent;
}

bool UTCFRTSCommandRouterComponent::ExecuteCommandIntent(FTCFRTSCommandIntent& CommandIntent) const
{
	if (!HasSelectedSquads())
	{
		CommandIntent.IntentType = ETCFRTSCommandIntentType::Invalid;
		return false;
	}

	switch (CommandIntent.IntentType)
	{
	case ETCFRTSCommandIntentType::Move:
		return MovementCommandComponent
			? MovementCommandComponent->MoveSelectedSquadsToLocation(CommandIntent.TargetLocation)
			: false;

	case ETCFRTSCommandIntentType::CapturePointMove:
		if (!bMoveToCapturePointOnCommand || !MovementCommandComponent)
		{
			return false;
		}

		return MovementCommandComponent->MoveSelectedSquadsToLocation(CommandIntent.TargetLocation);

	case ETCFRTSCommandIntentType::AttackTarget:
		// V2.2 will consume this intent and activate Basic Attack for selected squads.
		return false;

	case ETCFRTSCommandIntentType::GatherResource:
		// Worker gather command is a later V2 economy phase.
		return false;

	case ETCFRTSCommandIntentType::ProductionBuildingInteraction:
		// Building interaction/production UI is a later V2 phase.
		return false;

	case ETCFRTSCommandIntentType::None:
	case ETCFRTSCommandIntentType::Invalid:
	default:
		return false;
	}
}

ETCFRTSCommandIntentType UTCFRTSCommandRouterComponent::ResolveIntentType(const FTCFRTSHoverContext& HoverContext) const
{
	switch (HoverContext.TargetType)
	{
	case ETCFRTSHoverTargetType::Ground:
		return ETCFRTSCommandIntentType::Move;

	case ETCFRTSHoverTargetType::CapturePoint:
		return ETCFRTSCommandIntentType::CapturePointMove;

	case ETCFRTSHoverTargetType::ResourceNode:
		return ETCFRTSCommandIntentType::GatherResource;

	case ETCFRTSHoverTargetType::ProductionBuilding:
		return ETCFRTSCommandIntentType::ProductionBuildingInteraction;

	case ETCFRTSHoverTargetType::Squad:
		if (HoverContext.RelationshipToPrimarySelection == ETCFSquadRelationship::Enemy)
		{
			return ETCFRTSCommandIntentType::AttackTarget;
		}

		return ETCFRTSCommandIntentType::Invalid;

	case ETCFRTSHoverTargetType::OtherActor:
		return ETCFRTSCommandIntentType::Invalid;

	case ETCFRTSHoverTargetType::None:
	default:
		return ETCFRTSCommandIntentType::None;
	}
}

FVector UTCFRTSCommandRouterComponent::ResolveTargetLocation(const FTCFRTSHoverContext& HoverContext) const
{
	if (IsValid(HoverContext.HoveredActor)
		&& HoverContext.TargetType != ETCFRTSHoverTargetType::Ground)
	{
		return HoverContext.HoveredActor->GetActorLocation();
	}

	return HoverContext.WorldLocation;
}

int32 UTCFRTSCommandRouterComponent::GetSelectedSquadCount() const
{
	return SelectionComponent ? SelectionComponent->GetSelectedSquadCount() : 0;
}

bool UTCFRTSCommandRouterComponent::HasSelectedSquads() const
{
	return SelectionComponent && SelectionComponent->HasSelectedSquads();
}