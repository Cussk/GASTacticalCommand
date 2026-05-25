//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFSquadMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFSquadActor.h"
#include "GAS/TCFSquadAttributeSet.h"
#include "GameplayEffect.h"
#include "TCFGameplayTags.h"

UTCFSquadMovementComponent::UTCFSquadMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetIsReplicatedByDefault(false);
}

void UTCFSquadMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SquadOwner = Cast<ATCFSquadActor>(GetOwner());

	StartStationaryState();
}

void UTCFSquadMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearMovingState();
	ClearStationaryState();

	Super::EndPlay(EndPlayReason);
}

void UTCFSquadMovementComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bMoving || !SquadOwner)
	{
		SetComponentTickEnabled(false);
		return;
	}

	const FVector CurrentLocation = SquadOwner->GetActorLocation();
	const float DistanceSquared = FVector::DistSquared2D(CurrentLocation, TargetLocation);
	const float AcceptanceRadiusSquared = FMath::Square(AcceptanceRadius);

	if (DistanceSquared <= AcceptanceRadiusSquared)
	{
		FinishMovement();
		return;
	}

	const float MoveSpeed = GetCurrentMoveSpeed();
	if (MoveSpeed <= 0.0f)
	{
		return;
	}

	const FVector ToTarget = TargetLocation - CurrentLocation;
	const FVector Direction2D = FVector(ToTarget.X, ToTarget.Y, 0.0f).GetSafeNormal();

	if (Direction2D.IsNearlyZero())
	{
		FinishMovement();
		return;
	}

	FVector NewLocation = CurrentLocation + Direction2D * MoveSpeed * DeltaTime;

	const float RemainingDistance = FVector::Dist2D(CurrentLocation, TargetLocation);
	const float StepDistance = FVector::Dist2D(CurrentLocation, NewLocation);

	if (StepDistance >= RemainingDistance)
	{
		NewLocation = TargetLocation;
	}

	if (bKeepCurrentZ)
	{
		NewLocation.Z = CurrentLocation.Z;
	}

	SquadOwner->SetActorLocation(NewLocation, true);
}

bool UTCFSquadMovementComponent::MoveToLocation(FVector WorldLocation)
{
	if (!SquadOwner)
	{
		return false;
	}

	if (bKeepCurrentZ)
	{
		WorldLocation.Z = SquadOwner->GetActorLocation().Z;
	}

	TargetLocation = WorldLocation;

	const float DistanceSquared = FVector::DistSquared2D(SquadOwner->GetActorLocation(), TargetLocation);
	if (DistanceSquared <= FMath::Square(AcceptanceRadius))
	{
		FinishMovement();
		return true;
	}

	ClearStationaryState();
	StartMovingState();

	bMoving = true;
	SetComponentTickEnabled(true);

	OnMovementChanged.Broadcast(true);

	return true;
}

void UTCFSquadMovementComponent::StopMovement(bool bSnapToTarget)
{
	if (!SquadOwner)
	{
		return;
	}

	if (bSnapToTarget)
	{
		FVector NewLocation = TargetLocation;

		if (bKeepCurrentZ)
		{
			NewLocation.Z = SquadOwner->GetActorLocation().Z;
		}

		SquadOwner->SetActorLocation(NewLocation, true);
	}

	FinishMovement();
}

bool UTCFSquadMovementComponent::IsMoving() const
{
	return bMoving;
}

FVector UTCFSquadMovementComponent::GetMoveTargetLocation() const
{
	return TargetLocation;
}

void UTCFSquadMovementComponent::FinishMovement()
{
	if (!SquadOwner)
	{
		return;
	}

	bMoving = false;
	SetComponentTickEnabled(false);

	ClearMovingState();
	StartStationaryState();

	OnMovementChanged.Broadcast(false);
}

void UTCFSquadMovementComponent::StartMovingState()
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystem();
	if (!AbilitySystem)
	{
		return;
	}

	ClearMovingState();

	if (MovingStateEffect)
	{
		MovingStateEffectHandle = ApplyStateEffect(MovingStateEffect);
		return;
	}

	if (!bUsingLooseMovingTag)
	{
		AbilitySystem->AddLooseGameplayTag(TCFGameplayTags::Squad_State_Moving);
		bUsingLooseMovingTag = true;
	}
}

void UTCFSquadMovementComponent::StartStationaryState()
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystem();
	if (!AbilitySystem)
	{
		return;
	}

	ClearStationaryState();

	if (StationaryStateEffect)
	{
		StationaryStateEffectHandle = ApplyStateEffect(StationaryStateEffect);
		return;
	}

	if (!bUsingLooseStationaryTag)
	{
		AbilitySystem->AddLooseGameplayTag(TCFGameplayTags::Squad_State_Stationary);
		bUsingLooseStationaryTag = true;
	}
}

void UTCFSquadMovementComponent::ClearMovingState()
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystem();
	if (!AbilitySystem)
	{
		return;
	}

	if (MovingStateEffectHandle.IsValid())
	{
		AbilitySystem->RemoveActiveGameplayEffect(MovingStateEffectHandle);
		MovingStateEffectHandle.Invalidate();
	}

	if (bUsingLooseMovingTag)
	{
		AbilitySystem->RemoveLooseGameplayTag(TCFGameplayTags::Squad_State_Moving);
		bUsingLooseMovingTag = false;
	}
}

void UTCFSquadMovementComponent::ClearStationaryState()
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystem();
	if (!AbilitySystem)
	{
		return;
	}

	if (StationaryStateEffectHandle.IsValid())
	{
		AbilitySystem->RemoveActiveGameplayEffect(StationaryStateEffectHandle);
		StationaryStateEffectHandle.Invalidate();
	}

	if (bUsingLooseStationaryTag)
	{
		AbilitySystem->RemoveLooseGameplayTag(TCFGameplayTags::Squad_State_Stationary);
		bUsingLooseStationaryTag = false;
	}
}

float UTCFSquadMovementComponent::GetCurrentMoveSpeed() const
{
	if (!SquadOwner)
	{
		return FallbackMoveSpeed;
	}

	const UTCFSquadAttributeSet* AttributeSet = SquadOwner->GetSquadAttributeSet();
	if (!AttributeSet)
	{
		return FallbackMoveSpeed;
	}

	const float AttributeMoveSpeed = AttributeSet->GetMovementSpeed();
	return AttributeMoveSpeed > 0.0f ? AttributeMoveSpeed : FallbackMoveSpeed;
}

UAbilitySystemComponent* UTCFSquadMovementComponent::GetAbilitySystem() const
{
	return SquadOwner ? SquadOwner->GetAbilitySystemComponent() : nullptr;
}

FActiveGameplayEffectHandle UTCFSquadMovementComponent::ApplyStateEffect(TSubclassOf<UGameplayEffect> EffectClass) const
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystem();
	if (!AbilitySystem || !EffectClass)
	{
		return FActiveGameplayEffectHandle();
	}

	FGameplayEffectContextHandle ContextHandle = AbilitySystem->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(
		EffectClass,
		1.0f,
		ContextHandle);

	if (!SpecHandle.IsValid())
	{
		return FActiveGameplayEffectHandle();
	}

	return AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
