//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFRTSCommandRouterComponent.h"

#include "TCFGameplayTags.h"
#include "Actors/TCFBuildingActor.h"
#include "Actors/TCFResourceNodeActor.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerMovementCommandComponent.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Components/TCFRTSHoverContextComponent.h"
#include "Components/TCFRTSOrderTargetingComponent.h"
#include "Components/TCFSquadAttackCommandComponent.h"
#include "Components/TCFSquadBuildCommandComponent.h"
#include "Components/TCFSquadGatherCommandComponent.h"

UTCFRTSCommandRouterComponent::UTCFRTSCommandRouterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
	
	BasicAttackOrderTag = TCFGameplayTags::Order_Type_BasicAttack;
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
	OrderTargetingComponent = OwnerActor->FindComponentByClass<UTCFRTSOrderTargetingComponent>();
}

bool UTCFRTSCommandRouterComponent::ExecutePrimaryCommand()
{
	if (OrderTargetingComponent && OrderTargetingComponent->HasPendingOrder())
	{
		return OrderTargetingComponent->ConfirmPendingOrder();
	}
	
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
		StopSelectedSquadAttackCommands();
		StopSelectedSquadGatherCommands();
		StopSelectedSquadBuildCommands();

		return MovementCommandComponent
			? MovementCommandComponent->MoveSelectedSquadsToLocation(CommandIntent.TargetLocation)
			: false;

	case ETCFRTSCommandIntentType::CapturePointMove:
		if (!bMoveToCapturePointOnCommand || !MovementCommandComponent)
		{
			return false;
		}

		StopSelectedSquadAttackCommands();
		StopSelectedSquadGatherCommands();
		StopSelectedSquadBuildCommands();

		return MovementCommandComponent->MoveSelectedSquadsToLocation(CommandIntent.TargetLocation);

	case ETCFRTSCommandIntentType::AttackTarget:
		StopSelectedSquadGatherCommands();
		StopSelectedSquadBuildCommands();
		return ExecuteAttackTargetIntent(CommandIntent);

	case ETCFRTSCommandIntentType::GatherResource:
		StopSelectedSquadAttackCommands();
		StopSelectedSquadBuildCommands();
		return ExecuteGatherResourceIntent(CommandIntent);

	case ETCFRTSCommandIntentType::BuildTarget:
		StopSelectedSquadAttackCommands();
		StopSelectedSquadGatherCommands();
		return ExecuteBuildTargetIntent(CommandIntent);

	case ETCFRTSCommandIntentType::ProductionBuildingInteraction:
		// Building interaction/production UI is a later V2 phase.
		return false;

	case ETCFRTSCommandIntentType::None:
	case ETCFRTSCommandIntentType::Invalid:
	default:
		return false;
	}
}

bool UTCFRTSCommandRouterComponent::ExecuteAttackTargetIntent(const FTCFRTSCommandIntent& CommandIntent) const
{
	if (!SelectionComponent || !IsValid(CommandIntent.TargetActor))
	{
		return false;
	}

	TArray<ATCFSquadActor*> SelectedSquads;
	SelectionComponent->GetSelectedSquads(SelectedSquads);

	if (SelectedSquads.IsEmpty())
	{
		return false;
	}

	bool bStartedAnyAttackCommand = false;

	for (const ATCFSquadActor* SelectedSquad : SelectedSquads)
	{
		if (!IsValid(SelectedSquad))
		{
			continue;
		}

		UTCFSquadAttackCommandComponent* AttackCommandComponent = SelectedSquad->GetAttackCommandComponent();
		if (!AttackCommandComponent)
		{
			continue;
		}

		bStartedAnyAttackCommand |= AttackCommandComponent->StartAttackCommand(CommandIntent.TargetActor);
	}

	return bStartedAnyAttackCommand;
}

bool UTCFRTSCommandRouterComponent::ExecuteGatherResourceIntent(const FTCFRTSCommandIntent& CommandIntent) const
{
	if (!SelectionComponent || !IsValid(CommandIntent.TargetActor))
	{
		return false;
	}

	ATCFResourceNodeActor* ResourceNode = Cast<ATCFResourceNodeActor>(CommandIntent.TargetActor);
	if (!ResourceNode || !ResourceNode->CanGatherResource())
	{
		return false;
	}

	TArray<ATCFSquadActor*> SelectedSquads;
	SelectionComponent->GetSelectedSquads(SelectedSquads);

	if (SelectedSquads.IsEmpty())
	{
		return false;
	}

	bool bStartedAnyGatherCommand = false;

	for (ATCFSquadActor* SelectedSquad : SelectedSquads)
	{
		if (!IsValid(SelectedSquad))
		{
			continue;
		}

		if (UTCFSquadGatherCommandComponent* GatherCommandComponent = SelectedSquad->GetGatherCommandComponent())
		{
			bStartedAnyGatherCommand |= GatherCommandComponent->StartGatherCommand(ResourceNode);
		}
	}

	return bStartedAnyGatherCommand;
}

bool UTCFRTSCommandRouterComponent::ExecuteBuildTargetIntent(
	const FTCFRTSCommandIntent& CommandIntent) const
{
	if (!SelectionComponent || !IsValid(CommandIntent.TargetActor))
	{
		return false;
	}

	ATCFBuildingActor* Building = Cast<ATCFBuildingActor>(CommandIntent.TargetActor);
	if (!Building || !Building->CanReceiveConstructionWork())
	{
		return false;
	}

	if (CommandIntent.RelationshipToPrimarySelection != ETCFSquadRelationship::Own
		&& CommandIntent.RelationshipToPrimarySelection != ETCFSquadRelationship::Friendly)
	{
		return false;
	}

	TArray<ATCFSquadActor*> SelectedSquads;
	SelectionComponent->GetSelectedSquads(SelectedSquads);

	if (SelectedSquads.IsEmpty())
	{
		return false;
	}

	bool bStartedAnyBuildCommand = false;

	for (ATCFSquadActor* SelectedSquad : SelectedSquads)
	{
		if (!IsValid(SelectedSquad))
		{
			continue;
		}

		if (UTCFSquadBuildCommandComponent* BuildCommandComponent = SelectedSquad->GetBuildCommandComponent())
		{
			bStartedAnyBuildCommand |= BuildCommandComponent->StartBuildCommand(Building);
		}
	}

	return bStartedAnyBuildCommand;
}

void UTCFRTSCommandRouterComponent::StopSelectedSquadAttackCommands() const
{
	if (!SelectionComponent)
	{
		return;
	}

	TArray<ATCFSquadActor*> SelectedSquads;
	SelectionComponent->GetSelectedSquads(SelectedSquads);

	for (ATCFSquadActor* SelectedSquad : SelectedSquads)
	{
		if (!IsValid(SelectedSquad))
		{
			continue;
		}

		if (UTCFSquadAttackCommandComponent* AttackCommandComponent = SelectedSquad->GetAttackCommandComponent())
		{
			AttackCommandComponent->StopAttackCommand();
		}
	}
}

void UTCFRTSCommandRouterComponent::StopSelectedSquadGatherCommands() const
{
	if (!SelectionComponent)
	{
		return;
	}

	TArray<ATCFSquadActor*> SelectedSquads;
	SelectionComponent->GetSelectedSquads(SelectedSquads);

	for (ATCFSquadActor* SelectedSquad : SelectedSquads)
	{
		if (!IsValid(SelectedSquad))
		{
			continue;
		}

		if (UTCFSquadGatherCommandComponent* GatherCommandComponent = SelectedSquad->GetGatherCommandComponent())
		{
			GatherCommandComponent->StopGatherCommand();
		}
	}
}

void UTCFRTSCommandRouterComponent::StopSelectedSquadBuildCommands() const
{
	if (!SelectionComponent)
	{
		return;
	}

	TArray<ATCFSquadActor*> SelectedSquads;
	SelectionComponent->GetSelectedSquads(SelectedSquads);

	for (ATCFSquadActor* SelectedSquad : SelectedSquads)
	{
		if (!IsValid(SelectedSquad))
		{
			continue;
		}

		if (UTCFSquadBuildCommandComponent* BuildCommandComponent = SelectedSquad->GetBuildCommandComponent())
		{
			BuildCommandComponent->StopBuildCommand();
		}
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

	case ETCFRTSHoverTargetType::Building:
		{
			const ATCFBuildingActor* Building = Cast<ATCFBuildingActor>(HoverContext.HoveredActor);
			if (!Building)
			{
				return ETCFRTSCommandIntentType::Invalid;
			}

			if (Building->CanReceiveConstructionWork()
				&& (HoverContext.RelationshipToPrimarySelection == ETCFSquadRelationship::Own
					|| HoverContext.RelationshipToPrimarySelection == ETCFSquadRelationship::Friendly))
			{
				return ETCFRTSCommandIntentType::BuildTarget;
			}

			return ETCFRTSCommandIntentType::ProductionBuildingInteraction;
		}

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