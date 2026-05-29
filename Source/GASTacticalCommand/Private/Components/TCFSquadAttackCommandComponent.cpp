//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFSquadAttackCommandComponent.h"

#include "Actors/TCFSquadActor.h"
#include "Components/TCFSquadMovementComponent.h"
#include "Data/TCFOrderDefinition.h"
#include "Subsystems/TCFOrderSubsystem.h"
#include "Subsystems/TCFRelationshipSubsystem.h"
#include "TCFGameplayTags.h"
#include "TimerManager.h"

UTCFSquadAttackCommandComponent::UTCFSquadAttackCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);

	BasicAttackOrderTag = TCFGameplayTags::Order_Type_BasicAttack;
}

void UTCFSquadAttackCommandComponent::BeginPlay()
{
	Super::BeginPlay();

	SquadOwner = Cast<ATCFSquadActor>(GetOwner());
}

void UTCFSquadAttackCommandComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopAttackCommand();

	Super::EndPlay(EndPlayReason);
}

bool UTCFSquadAttackCommandComponent::StartAttackCommand(AActor* NewTargetActor)
{
	if (!SquadOwner || !IsValid(NewTargetActor) || NewTargetActor == SquadOwner)
	{
		return false;
	}

	StopAttackCommand();

	TargetActor = NewTargetActor;
	BindTargetDestroyed(NewTargetActor);

	if (!IsAttackCommandStillValid())
	{
		StopAttackCommand();
		return false;
	}

	bHasIssuedMove = false;
	LastMoveTargetLocation = FVector::ZeroVector;

	TickAttackCommand();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			AttackCommandTimerHandle,
			this,
			&UTCFSquadAttackCommandComponent::TickAttackCommand,
			CommandUpdateInterval,
			true);
	}

	OnAttackCommandChanged.Broadcast(true, TargetActor);
	return true;
}

void UTCFSquadAttackCommandComponent::StopAttackCommand()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AttackCommandTimerHandle);
	}

	UnbindTargetDestroyed();

	AActor* PreviousTarget = TargetActor;
	TargetActor = nullptr;
	bHasIssuedMove = false;
	LastMoveTargetLocation = FVector::ZeroVector;

	if (PreviousTarget)
	{
		OnAttackCommandChanged.Broadcast(false, nullptr);
	}
}

bool UTCFSquadAttackCommandComponent::HasAttackCommand() const
{
	return IsValid(TargetActor);
}

AActor* UTCFSquadAttackCommandComponent::GetAttackTarget() const
{
	return IsValid(TargetActor) ? TargetActor : nullptr;
}

void UTCFSquadAttackCommandComponent::TickAttackCommand()
{
	if (!IsAttackCommandStillValid())
	{
		StopAttackCommand();
		return;
	}

	if (IsTargetInAttackRange())
	{
		StopMovement();
		SubmitBasicAttackOrder();
		return;
	}

	MoveTowardTarget();
}

bool UTCFSquadAttackCommandComponent::IsAttackCommandStillValid() const
{
	return IsValid(SquadOwner)
		&& IsValid(TargetActor)
		&& BasicAttackOrderTag.IsValid()
		&& IsTargetEnemy();
}

bool UTCFSquadAttackCommandComponent::IsTargetEnemy() const
{
	const UWorld* World = GetWorld();
	const UTCFRelationshipSubsystem* RelationshipSubsystem = World
		? World->GetSubsystem<UTCFRelationshipSubsystem>()
		: nullptr;

	if (!RelationshipSubsystem || !SquadOwner || !TargetActor)
	{
		return false;
	}

	return RelationshipSubsystem->GetActorRelationship(SquadOwner, TargetActor) == ETCFSquadRelationship::Enemy;
}

bool UTCFSquadAttackCommandComponent::IsTargetInAttackRange() const
{
	if (!SquadOwner || !TargetActor)
	{
		return false;
	}

	const float AttackRange = GetAttackRange();
	if (AttackRange <= 0.0f)
	{
		return true;
	}

	const float DistanceSquared = FVector::DistSquared2D(
		SquadOwner->GetActorLocation(),
		TargetActor->GetActorLocation());

	return DistanceSquared <= FMath::Square(AttackRange);
}

float UTCFSquadAttackCommandComponent::GetAttackRange() const
{
	const UWorld* World = GetWorld();
	const UTCFOrderSubsystem* OrderSubsystem = World
		? World->GetSubsystem<UTCFOrderSubsystem>()
		: nullptr;

	const UTCFOrderDefinition* BasicAttackDefinition = OrderSubsystem
		? OrderSubsystem->GetOrderDefinition(BasicAttackOrderTag)
		: nullptr;

	if (BasicAttackDefinition && BasicAttackDefinition->Targeting.Range > 0.0f)
	{
		return BasicAttackDefinition->Targeting.Range;
	}

	return FallbackAttackRange;
}

FVector UTCFSquadAttackCommandComponent::GetDesiredAttackMoveLocation() const
{
	if (!SquadOwner || !TargetActor)
	{
		return FVector::ZeroVector;
	}

	const FVector SourceLocation = SquadOwner->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();

	FVector DirectionFromTargetToSource = SourceLocation - TargetLocation;
	DirectionFromTargetToSource.Z = 0.0f;

	if (DirectionFromTargetToSource.IsNearlyZero())
	{
		DirectionFromTargetToSource = FVector::ForwardVector;
	}
	else
	{
		DirectionFromTargetToSource.Normalize();
	}

	const float DesiredDistance = FMath::Max(0.0f, GetAttackRange() - AttackRangePadding);
	FVector MoveLocation = TargetLocation + DirectionFromTargetToSource * DesiredDistance;
	MoveLocation.Z = SourceLocation.Z;

	return MoveLocation;
}

bool UTCFSquadAttackCommandComponent::SubmitBasicAttackOrder() const
{
	if (!SquadOwner || !TargetActor)
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
	OrderTarget.TargetActor = TargetActor;
	OrderTarget.TargetLocation = TargetActor->GetActorLocation();

	FTCFSquadOrderRequest Request;
	Request.OrderTag = BasicAttackOrderTag;
	Request.SourceActor = SquadOwner;
	Request.Target = OrderTarget;

	const FTCFOrderResult Result = OrderSubsystem->SubmitOrder(Request);
	return Result.bSuccess;
}

void UTCFSquadAttackCommandComponent::MoveTowardTarget()
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

	const FVector DesiredMoveLocation = GetDesiredAttackMoveLocation();

	if (bHasIssuedMove
		&& FVector::DistSquared2D(DesiredMoveLocation, LastMoveTargetLocation) <= FMath::Square(MoveTargetRefreshDistance))
	{
		return;
	}

	bHasIssuedMove = true;
	LastMoveTargetLocation = DesiredMoveLocation;

	MovementComponent->MoveToLocation(DesiredMoveLocation);
}

void UTCFSquadAttackCommandComponent::StopMovement() const
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

void UTCFSquadAttackCommandComponent::BindTargetDestroyed(AActor* NewTargetActor)
{
	if (IsValid(NewTargetActor))
	{
		NewTargetActor->OnDestroyed.AddDynamic(this, &UTCFSquadAttackCommandComponent::HandleTargetDestroyed);
	}
}

void UTCFSquadAttackCommandComponent::UnbindTargetDestroyed()
{
	if (IsValid(TargetActor))
	{
		TargetActor->OnDestroyed.RemoveDynamic(this, &UTCFSquadAttackCommandComponent::HandleTargetDestroyed);
	}
}

void UTCFSquadAttackCommandComponent::HandleTargetDestroyed(AActor* DestroyedActor)
{
	if (DestroyedActor == TargetActor)
	{
		StopAttackCommand();
	}
}