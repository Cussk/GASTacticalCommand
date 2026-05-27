//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFSquadIntegrityStateComponent.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFSquadMovementComponent.h"
#include "GAS/TCFSquadAttributeSet.h"
#include "TCFGameplayTags.h"
#include "TimerManager.h"

UTCFSquadIntegrityStateComponent::UTCFSquadIntegrityStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFSquadIntegrityStateComponent::BeginPlay()
{
	Super::BeginPlay();

	SquadOwner = Cast<ATCFSquadActor>(GetOwner());
	if (!SquadOwner)
	{
		return;
	}

	AbilitySystem = SquadOwner->GetAbilitySystemComponent();
	AttributeSet = SquadOwner->GetSquadAttributeSet();

	BindAttributeDelegates();
	EvaluateInitialIntegrityState();
}

void UTCFSquadIntegrityStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindAttributeDelegates();

	Super::EndPlay(EndPlayReason);
}

bool UTCFSquadIntegrityStateComponent::IsDefeated() const
{
	return bDefeated;
}

void UTCFSquadIntegrityStateComponent::ForceDefeat()
{
	EnterDefeatedState();
}

void UTCFSquadIntegrityStateComponent::BindAttributeDelegates()
{
	if (!AbilitySystem || !AttributeSet)
	{
		return;
	}

	HealthChangedDelegateHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(
		UTCFSquadAttributeSet::GetHealthAttribute()).AddUObject(
			this,
			&UTCFSquadIntegrityStateComponent::HandleHealthChanged);

	CohesionChangedDelegateHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(
		UTCFSquadAttributeSet::GetCohesionAttribute()).AddUObject(
			this,
			&UTCFSquadIntegrityStateComponent::HandleCohesionChanged);

	MoraleChangedDelegateHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(
		UTCFSquadAttributeSet::GetMoraleAttribute()).AddUObject(
			this,
			&UTCFSquadIntegrityStateComponent::HandleMoraleChanged);
}

void UTCFSquadIntegrityStateComponent::UnbindAttributeDelegates()
{
	if (!AbilitySystem)
	{
		return;
	}

	if (HealthChangedDelegateHandle.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(
			UTCFSquadAttributeSet::GetHealthAttribute()).Remove(HealthChangedDelegateHandle);
		HealthChangedDelegateHandle.Reset();
	}

	if (CohesionChangedDelegateHandle.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(
			UTCFSquadAttributeSet::GetCohesionAttribute()).Remove(CohesionChangedDelegateHandle);
		CohesionChangedDelegateHandle.Reset();
	}

	if (MoraleChangedDelegateHandle.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(
			UTCFSquadAttributeSet::GetMoraleAttribute()).Remove(MoraleChangedDelegateHandle);
		MoraleChangedDelegateHandle.Reset();
	}
}

void UTCFSquadIntegrityStateComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.NewValue <= 0.0f)
	{
		EnterDefeatedState();
	}
}

void UTCFSquadIntegrityStateComponent::HandleCohesionChanged(const FOnAttributeChangeData& ChangeData)
{
	// Future: update squad member visibility/count, footprint, and cohesion threshold tags.
}

void UTCFSquadIntegrityStateComponent::HandleMoraleChanged(const FOnAttributeChangeData& ChangeData)
{
	// Future: update morale threshold tags/effects such as shaken, broken, or routed.
}

void UTCFSquadIntegrityStateComponent::EvaluateInitialIntegrityState()
{
	if (AttributeSet && AttributeSet->GetHealth() <= 0.0f)
	{
		EnterDefeatedState();
	}
}

void UTCFSquadIntegrityStateComponent::EnterDefeatedState()
{
	if (bDefeated)
	{
		return;
	}

	bDefeated = true;

	StopMovement();
	CancelActiveAbilities();
	ApplyDeadTag();

	OnSquadDefeated.Broadcast();

	if (bDestroyActorOnDefeat)
	{
		ScheduleDestroy();
	}
}

void UTCFSquadIntegrityStateComponent::StopMovement() const
{
	if (!SquadOwner)
	{
		return;
	}

	if (UTCFSquadMovementComponent* MovementComponent = SquadOwner->GetMovementComponent())
	{
		MovementComponent->StopMovement(false);
	}
}

void UTCFSquadIntegrityStateComponent::CancelActiveAbilities() const
{
	if (!AbilitySystem)
	{
		return;
	}

	AbilitySystem->CancelAllAbilities();
}

void UTCFSquadIntegrityStateComponent::ApplyDeadTag() const
{
	if (!AbilitySystem)
	{
		return;
	}

	AbilitySystem->AddLooseGameplayTag(TCFGameplayTags::Squad_State_Dead);
}

void UTCFSquadIntegrityStateComponent::ScheduleDestroy() const
{
	if (!SquadOwner)
	{
		return;
	}

	if (DestroyDelaySeconds <= 0.0f)
	{
		SquadOwner->Destroy();
		return;
	}

	FTimerDelegate DestroyDelegate;
	DestroyDelegate.BindWeakLambda(SquadOwner, [WeakSquad = TWeakObjectPtr<ATCFSquadActor>(SquadOwner)]()
	{
		if (ATCFSquadActor* Squad = WeakSquad.Get())
		{
			Squad->Destroy();
		}
	});

	if (UWorld* World = GetWorld())
	{
		FTimerHandle DestroyTimerHandle;
		World->GetTimerManager().SetTimer(
			DestroyTimerHandle,
			DestroyDelegate,
			DestroyDelaySeconds,
			false);
	}
}
