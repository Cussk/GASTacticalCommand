//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFSquadGatherCommandComponent.h"

#include "Actors/TCFResourceNodeActor.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFSquadMovementComponent.h"
#include "Data/TCFOrderDefinition.h"
#include "Subsystems/TCFOrderSubsystem.h"
#include "TCFGameplayTags.h"
#include "TimerManager.h"

UTCFSquadGatherCommandComponent::UTCFSquadGatherCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);

	GatherOrderTag = TCFGameplayTags::Order_Type_Gather;
	WorkerRoleTag = TCFGameplayTags::Squad_Role_Worker;
}

void UTCFSquadGatherCommandComponent::BeginPlay()
{
	Super::BeginPlay();

	SquadOwner = Cast<ATCFSquadActor>(GetOwner());
}

void UTCFSquadGatherCommandComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopGatherCommand();

	Super::EndPlay(EndPlayReason);
}

bool UTCFSquadGatherCommandComponent::StartGatherCommand(ATCFResourceNodeActor* NewResourceNode)
{
	if (!SquadOwner || !IsValid(NewResourceNode))
	{
		return false;
	}

	StopGatherCommand();

	TargetResourceNode = NewResourceNode;
	BindResourceNodeDestroyed(NewResourceNode);

	if (!IsGatherCommandStillValid())
	{
		StopGatherCommand();
		return false;
	}

	bHasIssuedMove = false;
	LastMoveTargetLocation = FVector::ZeroVector;

	TickGatherCommand();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			GatherCommandTimerHandle,
			this,
			&UTCFSquadGatherCommandComponent::TickGatherCommand,
			CommandUpdateInterval,
			true);
	}

	OnGatherCommandChanged.Broadcast(true, TargetResourceNode);
	return true;
}

void UTCFSquadGatherCommandComponent::StopGatherCommand()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(GatherCommandTimerHandle);
	}

	UnbindResourceNodeDestroyed();

	ATCFResourceNodeActor* PreviousTarget = TargetResourceNode;
	TargetResourceNode = nullptr;
	bHasIssuedMove = false;
	LastMoveTargetLocation = FVector::ZeroVector;

	if (PreviousTarget)
	{
		OnGatherCommandChanged.Broadcast(false, nullptr);
	}
}

bool UTCFSquadGatherCommandComponent::HasGatherCommand() const
{
	return IsValid(TargetResourceNode);
}

ATCFResourceNodeActor* UTCFSquadGatherCommandComponent::GetTargetResourceNode() const
{
	return IsValid(TargetResourceNode) ? TargetResourceNode : nullptr;
}

void UTCFSquadGatherCommandComponent::TickGatherCommand()
{
	if (!IsGatherCommandStillValid())
	{
		StopGatherCommand();
		return;
	}

	if (IsTargetInGatherRange())
	{
		StopMovement();
		SubmitGatherOrder();
		return;
	}

	MoveTowardResourceNode();
}

bool UTCFSquadGatherCommandComponent::IsGatherCommandStillValid() const
{
	return IsValid(SquadOwner)
		&& IsValid(TargetResourceNode)
		&& GatherOrderTag.IsValid()
		&& IsWorkerSquad()
		&& TargetResourceNode->CanGatherResource();
}

bool UTCFSquadGatherCommandComponent::IsWorkerSquad() const
{
	return SquadOwner
		&& WorkerRoleTag.IsValid()
		&& SquadOwner->GetRoleTag().MatchesTagExact(WorkerRoleTag);
}

bool UTCFSquadGatherCommandComponent::IsTargetInGatherRange() const
{
	if (!SquadOwner || !TargetResourceNode)
	{
		return false;
	}

	const float GatherRange = GetGatherRange();
	if (GatherRange <= 0.0f)
	{
		return true;
	}

	return GetDistanceToResourceInteractionBounds2D() <= GatherRange;
}

FVector UTCFSquadGatherCommandComponent::GetClosestResourceInteractionPointToSquad() const
{
	if (!SquadOwner || !TargetResourceNode)
	{
		return FVector::ZeroVector;
	}

	const UPrimitiveComponent* InteractionComponent = TargetResourceNode->GetInteractionCollisionComponent();
	if (!InteractionComponent)
	{
		FVector FallbackLocation = TargetResourceNode->GetActorLocation();
		FallbackLocation.Z = SquadOwner->GetActorLocation().Z;
		return FallbackLocation;
	}

	const FVector SourceLocation = SquadOwner->GetActorLocation();

	FVector ClosestPoint = InteractionComponent->Bounds.GetBox().GetClosestPointTo(SourceLocation);
	ClosestPoint.Z = SourceLocation.Z;

	return ClosestPoint;
}

float UTCFSquadGatherCommandComponent::GetDistanceToResourceInteractionBounds2D() const
{
	if (!SquadOwner || !TargetResourceNode)
	{
		return TNumericLimits<float>::Max();
	}

	return FVector::Dist2D(
		SquadOwner->GetActorLocation(),
		GetClosestResourceInteractionPointToSquad());
}

float UTCFSquadGatherCommandComponent::GetGatherRange() const
{
	const UWorld* World = GetWorld();
	const UTCFOrderSubsystem* OrderSubsystem = World
		? World->GetSubsystem<UTCFOrderSubsystem>()
		: nullptr;

	const UTCFOrderDefinition* GatherDefinition = OrderSubsystem
		? OrderSubsystem->GetOrderDefinition(GatherOrderTag)
		: nullptr;

	if (GatherDefinition && GatherDefinition->Targeting.Range > 0.0f)
	{
		return GatherDefinition->Targeting.Range;
	}

	return FallbackGatherRange;
}

FVector UTCFSquadGatherCommandComponent::GetDesiredGatherMoveLocation() const
{
	if (!SquadOwner || !TargetResourceNode)
	{
		return FVector::ZeroVector;
	}

	const FVector SourceLocation = SquadOwner->GetActorLocation();
	const FVector TargetLocation = GetClosestResourceInteractionPointToSquad();

	FVector DirectionFromTargetToSource = SourceLocation - TargetLocation;
	DirectionFromTargetToSource.Z = 0.0f;

	if (DirectionFromTargetToSource.IsNearlyZero())
	{
		DirectionFromTargetToSource = SourceLocation - TargetResourceNode->GetActorLocation();
		DirectionFromTargetToSource.Z = 0.0f;
	}

	if (DirectionFromTargetToSource.IsNearlyZero())
	{
		DirectionFromTargetToSource = FVector::ForwardVector;
	}
	else
	{
		DirectionFromTargetToSource.Normalize();
	}

	const float DesiredDistance = FMath::Max(0.0f, GetGatherRange() - GatherRangePadding);

	FVector MoveLocation = TargetLocation + DirectionFromTargetToSource * DesiredDistance;
	MoveLocation.Z = SourceLocation.Z;

	return MoveLocation;
}

bool UTCFSquadGatherCommandComponent::SubmitGatherOrder() const
{
	if (!SquadOwner || !TargetResourceNode)
	{
		return false;
	}

	UWorld* World = GetWorld();
	UTCFOrderSubsystem* OrderSubsystem = World
		? World->GetSubsystem<UTCFOrderSubsystem>()
		: nullptr;

	if (!OrderSubsystem)
	{
		return false;
	}

	FTCFOrderTarget OrderTarget;
	OrderTarget.TargetType = ETCFOrderTargetType::Actor;
	OrderTarget.TargetActor = TargetResourceNode;
	OrderTarget.TargetLocation = GetClosestResourceInteractionPointToSquad();
	OrderTarget.bUseTargetLocationForRange = true;

	FTCFSquadOrderRequest Request;
	Request.OrderTag = GatherOrderTag;
	Request.SourceActor = SquadOwner;
	Request.Target = OrderTarget;

	const FTCFOrderResult Result = OrderSubsystem->SubmitOrder(Request);
	return Result.bSuccess;
}

void UTCFSquadGatherCommandComponent::MoveTowardResourceNode()
{
	if (!SquadOwner)
	{
		return;
	}

	UTCFSquadMovementComponent* MovementComponent = SquadOwner->GetMovementComponent();
	if (!MovementComponent)
	{
		return;
	}

	const FVector DesiredMoveLocation = GetDesiredGatherMoveLocation();

	if (bHasIssuedMove
		&& FVector::DistSquared2D(DesiredMoveLocation, LastMoveTargetLocation) <= FMath::Square(MoveTargetRefreshDistance))
	{
		return;
	}

	bHasIssuedMove = true;
	LastMoveTargetLocation = DesiredMoveLocation;

	MovementComponent->MoveToLocation(DesiredMoveLocation);
}

void UTCFSquadGatherCommandComponent::StopMovement() const
{
	if (!SquadOwner)
	{
		return;
	}

	if (UTCFSquadMovementComponent* MovementComponent = SquadOwner->GetMovementComponent())
	{
		if (MovementComponent->IsMoving())
		{
			MovementComponent->StopMovement(false);
		}
	}
}

void UTCFSquadGatherCommandComponent::BindResourceNodeDestroyed(ATCFResourceNodeActor* NewResourceNode)
{
	if (IsValid(NewResourceNode))
	{
		NewResourceNode->OnDestroyed.AddDynamic(this, &UTCFSquadGatherCommandComponent::HandleResourceNodeDestroyed);
	}
}

void UTCFSquadGatherCommandComponent::UnbindResourceNodeDestroyed()
{
	if (IsValid(TargetResourceNode))
	{
		TargetResourceNode->OnDestroyed.RemoveDynamic(this, &UTCFSquadGatherCommandComponent::HandleResourceNodeDestroyed);
	}
}

void UTCFSquadGatherCommandComponent::HandleResourceNodeDestroyed(AActor* DestroyedActor)
{
	if (DestroyedActor == TargetResourceNode)
	{
		StopGatherCommand();
	}
}