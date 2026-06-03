//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFSquadBuildCommandComponent.h"

#include "Actors/TCFBuildingActor.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFSquadMovementComponent.h"
#include "Data/TCFOrderDefinition.h"
#include "Subsystems/TCFOrderSubsystem.h"
#include "Subsystems/TCFRelationshipSubsystem.h"
#include "TCFGameplayTags.h"
#include "TimerManager.h"

UTCFSquadBuildCommandComponent::UTCFSquadBuildCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);

	BuildOrderTag = TCFGameplayTags::Order_Type_Build;
	WorkerRoleTag = TCFGameplayTags::Squad_Role_Worker;
}

void UTCFSquadBuildCommandComponent::BeginPlay()
{
	Super::BeginPlay();

	SquadOwner = Cast<ATCFSquadActor>(GetOwner());
}

void UTCFSquadBuildCommandComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopBuildCommand();

	Super::EndPlay(EndPlayReason);
}

bool UTCFSquadBuildCommandComponent::StartBuildCommand(ATCFBuildingActor* NewBuilding)
{
	if (!SquadOwner || !IsValid(NewBuilding))
	{
		return false;
	}

	StopBuildCommand();

	TargetBuilding = NewBuilding;
	BindBuildingDestroyed(NewBuilding);

	if (!IsBuildCommandStillValid())
	{
		StopBuildCommand();
		return false;
	}

	bHasIssuedMove = false;
	LastMoveTargetLocation = FVector::ZeroVector;

	TickBuildCommand();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			BuildCommandTimerHandle,
			this,
			&UTCFSquadBuildCommandComponent::TickBuildCommand,
			CommandUpdateInterval,
			true);
	}

	OnBuildCommandChanged.Broadcast(true, TargetBuilding);
	return true;
}

void UTCFSquadBuildCommandComponent::StopBuildCommand()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BuildCommandTimerHandle);
	}

	UnbindBuildingDestroyed();

	ATCFBuildingActor* PreviousTarget = TargetBuilding;
	TargetBuilding = nullptr;
	bHasIssuedMove = false;
	LastMoveTargetLocation = FVector::ZeroVector;

	if (PreviousTarget)
	{
		OnBuildCommandChanged.Broadcast(false, nullptr);
	}
}

bool UTCFSquadBuildCommandComponent::HasBuildCommand() const
{
	return IsValid(TargetBuilding);
}

ATCFBuildingActor* UTCFSquadBuildCommandComponent::GetTargetBuilding() const
{
	return IsValid(TargetBuilding) ? TargetBuilding : nullptr;
}

void UTCFSquadBuildCommandComponent::TickBuildCommand()
{
	if (!IsBuildCommandStillValid())
	{
		StopBuildCommand();
		return;
	}

	if (IsTargetInBuildRange())
	{
		StopMovement();
		SubmitBuildOrder();
		return;
	}

	MoveTowardBuilding();
}

bool UTCFSquadBuildCommandComponent::IsBuildCommandStillValid() const
{
	return IsValid(SquadOwner)
		&& IsValid(TargetBuilding)
		&& BuildOrderTag.IsValid()
		&& IsWorkerSquad()
		&& IsBuildTargetRelationshipAllowed()
		&& TargetBuilding->CanReceiveConstructionWork();
}

bool UTCFSquadBuildCommandComponent::IsWorkerSquad() const
{
	return SquadOwner
		&& WorkerRoleTag.IsValid()
		&& SquadOwner->GetRoleTag().MatchesTagExact(WorkerRoleTag);
}

bool UTCFSquadBuildCommandComponent::IsBuildTargetRelationshipAllowed() const
{
	const UWorld* World = GetWorld();
	const UTCFRelationshipSubsystem* RelationshipSubsystem = World
		? World->GetSubsystem<UTCFRelationshipSubsystem>()
		: nullptr;

	if (!RelationshipSubsystem || !SquadOwner || !TargetBuilding)
	{
		return false;
	}

	const ETCFSquadRelationship Relationship = RelationshipSubsystem->GetActorRelationship(
		SquadOwner,
		TargetBuilding);

	return Relationship == ETCFSquadRelationship::Own
		|| Relationship == ETCFSquadRelationship::Friendly;
}

bool UTCFSquadBuildCommandComponent::IsTargetInBuildRange() const
{
	if (!SquadOwner || !TargetBuilding)
	{
		return false;
	}

	const float BuildRange = GetBuildRange();
	if (BuildRange <= 0.0f)
	{
		return true;
	}

	const float DistanceSquared = FVector::DistSquared2D(
		SquadOwner->GetActorLocation(),
		TargetBuilding->GetActorLocation());

	return DistanceSquared <= FMath::Square(BuildRange);
}

float UTCFSquadBuildCommandComponent::GetBuildRange() const
{
	const UWorld* World = GetWorld();
	const UTCFOrderSubsystem* OrderSubsystem = World
		? World->GetSubsystem<UTCFOrderSubsystem>()
		: nullptr;

	const UTCFOrderDefinition* BuildDefinition = OrderSubsystem
		? OrderSubsystem->GetOrderDefinition(BuildOrderTag)
		: nullptr;

	if (BuildDefinition && BuildDefinition->Targeting.Range > 0.0f)
	{
		return BuildDefinition->Targeting.Range;
	}

	return FallbackBuildRange;
}

FVector UTCFSquadBuildCommandComponent::GetDesiredBuildMoveLocation() const
{
	if (!SquadOwner || !TargetBuilding)
	{
		return FVector::ZeroVector;
	}

	const FVector SourceLocation = SquadOwner->GetActorLocation();
	const FVector TargetLocation = TargetBuilding->GetActorLocation();

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

	const float DesiredDistance = FMath::Max(0.0f, GetBuildRange() - BuildRangePadding);
	FVector MoveLocation = TargetLocation + DirectionFromTargetToSource * DesiredDistance;
	MoveLocation.Z = SourceLocation.Z;

	return MoveLocation;
}

bool UTCFSquadBuildCommandComponent::SubmitBuildOrder() const
{
	if (!SquadOwner || !TargetBuilding)
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
	OrderTarget.TargetActor = TargetBuilding;
	OrderTarget.TargetLocation = TargetBuilding->GetActorLocation();

	FTCFSquadOrderRequest Request;
	Request.OrderTag = BuildOrderTag;
	Request.SourceActor = SquadOwner;
	Request.Target = OrderTarget;

	const FTCFOrderResult Result = OrderSubsystem->SubmitOrder(Request);
	return Result.bSuccess;
}

void UTCFSquadBuildCommandComponent::MoveTowardBuilding()
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

	const FVector DesiredMoveLocation = GetDesiredBuildMoveLocation();

	if (bHasIssuedMove
		&& FVector::DistSquared2D(DesiredMoveLocation, LastMoveTargetLocation) <= FMath::Square(MoveTargetRefreshDistance))
	{
		return;
	}

	bHasIssuedMove = true;
	LastMoveTargetLocation = DesiredMoveLocation;

	MovementComponent->MoveToLocation(DesiredMoveLocation);
}

void UTCFSquadBuildCommandComponent::StopMovement() const
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

void UTCFSquadBuildCommandComponent::BindBuildingDestroyed(ATCFBuildingActor* NewBuilding)
{
	if (IsValid(NewBuilding))
	{
		NewBuilding->OnDestroyed.AddDynamic(this, &UTCFSquadBuildCommandComponent::HandleBuildingDestroyed);
		NewBuilding->OnConstructionCompleted.AddDynamic(this, &UTCFSquadBuildCommandComponent::HandleBuildingConstructionCompleted);
	}
}

void UTCFSquadBuildCommandComponent::UnbindBuildingDestroyed()
{
	if (IsValid(TargetBuilding))
	{
		TargetBuilding->OnDestroyed.RemoveDynamic(this, &UTCFSquadBuildCommandComponent::HandleBuildingDestroyed);
		TargetBuilding->OnConstructionCompleted.RemoveDynamic(this, &UTCFSquadBuildCommandComponent::HandleBuildingConstructionCompleted);
	}
}

void UTCFSquadBuildCommandComponent::HandleBuildingDestroyed(AActor* DestroyedActor)
{
	if (DestroyedActor == TargetBuilding)
	{
		StopBuildCommand();
	}
}

void UTCFSquadBuildCommandComponent::HandleBuildingConstructionCompleted(
	ATCFBuildingActor* Building,
	AActor* CompletionSource)
{
	if (Building == TargetBuilding)
	{
		StopBuildCommand();
	}
}