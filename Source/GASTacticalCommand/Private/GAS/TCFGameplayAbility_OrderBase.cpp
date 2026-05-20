//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/TCFGameplayAbility_OrderBase.h"

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