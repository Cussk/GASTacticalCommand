//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_OrderBase.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFSquadActor.h"
#include "Data/TCFOrderDefinition.h"
#include "GAS/TCFOrderPayload.h"

UTCFGameplayAbility_OrderBase::UTCFGameplayAbility_OrderBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

FTCFSquadOrderRequest UTCFGameplayAbility_OrderBase::GetCurrentOrderRequest() const
{
	return CurrentOrderRequest;
}

UTCFOrderDefinition* UTCFGameplayAbility_OrderBase::GetCurrentOrderDefinition() const
{
	return CurrentOrderDefinition;
}

ATCFSquadActor* UTCFGameplayAbility_OrderBase::GetSourceSquad() const
{
	return Cast<ATCFSquadActor>(CurrentOrderRequest.SourceActor);
}

void UTCFGameplayAbility_OrderBase::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CaptureOrderPayload(TriggerEventData))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (bCommitAbilityOnOrderActivated && !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	HandleOrderActivated();

	if (bEndAbilityAfterOrderActivated)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UTCFGameplayAbility_OrderBase::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	ClearOrderData();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UTCFGameplayAbility_OrderBase::ApplyGameplayEffectClassToSelf(
	TSubclassOf<UGameplayEffect> EffectClass,
	float EffectLevel,
	UObject* SourceObject) const
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystem || !EffectClass)
	{
		return false;
	}

	FGameplayEffectContextHandle ContextHandle = AbilitySystem->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceObject ? SourceObject : const_cast<UTCFGameplayAbility_OrderBase*>(this));

	const FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(
		EffectClass,
		EffectLevel,
		ContextHandle);

	if (!SpecHandle.IsValid())
	{
		return false;
	}

	AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return true;
}

int32 UTCFGameplayAbility_OrderBase::ApplyGameplayEffectsToSelf(
	const TArray<TSubclassOf<UGameplayEffect>>& EffectClasses,
	float EffectLevel,
	UObject* SourceObject) const
{
	int32 AppliedCount = 0;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : EffectClasses)
	{
		if (ApplyGameplayEffectClassToSelf(EffectClass, EffectLevel, SourceObject))
		{
			AppliedCount++;
		}
	}

	return AppliedCount;
}

bool UTCFGameplayAbility_OrderBase::ApplyGameplayEffectClassToTarget(
	UAbilitySystemComponent* TargetAbilitySystem,
	TSubclassOf<UGameplayEffect> EffectClass,
	float EffectLevel,
	UObject* SourceObject) const
{
	UAbilitySystemComponent* SourceAbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (!SourceAbilitySystem || !TargetAbilitySystem || !EffectClass)
	{
		return false;
	}

	FGameplayEffectContextHandle ContextHandle = SourceAbilitySystem->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceObject ? SourceObject : const_cast<UTCFGameplayAbility_OrderBase*>(this));

	const FGameplayEffectSpecHandle SpecHandle = SourceAbilitySystem->MakeOutgoingSpec(
		EffectClass,
		EffectLevel,
		ContextHandle);

	if (!SpecHandle.IsValid())
	{
		return false;
	}

	SourceAbilitySystem->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetAbilitySystem);
	return true;
}

int32 UTCFGameplayAbility_OrderBase::ApplyGameplayEffectsToTarget(
	UAbilitySystemComponent* TargetAbilitySystem,
	const TArray<TSubclassOf<UGameplayEffect>>& EffectClasses,
	float EffectLevel,
	UObject* SourceObject) const
{
	int32 AppliedCount = 0;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : EffectClasses)
	{
		if (ApplyGameplayEffectClassToTarget(TargetAbilitySystem, EffectClass, EffectLevel, SourceObject))
		{
			AppliedCount++;
		}
	}

	return AppliedCount;
}

void UTCFGameplayAbility_OrderBase::HandleOrderActivated()
{
	ReceiveOrderActivated(CurrentOrderRequest, CurrentOrderDefinition);
}

bool UTCFGameplayAbility_OrderBase::CaptureOrderPayload(const FGameplayEventData* TriggerEventData)
{
	if (!TriggerEventData)
	{
		return false;
	}

	const UTCFOrderPayload* Payload = Cast<UTCFOrderPayload>(TriggerEventData->OptionalObject.Get());
	if (!Payload || !Payload->IsValidPayload())
	{
		return false;
	}

	CurrentOrderRequest = Payload->GetRequest();
	CurrentOrderDefinition = Payload->GetOrderDefinition();

	return CurrentOrderRequest.IsValidRequest() && IsValid(CurrentOrderDefinition);
}

void UTCFGameplayAbility_OrderBase::ClearOrderData()
{
	CurrentOrderRequest = FTCFSquadOrderRequest();
	CurrentOrderDefinition = nullptr;
}
