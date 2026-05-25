//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Types/TCFOrderTypes.h"
#include "TCFGameplayAbility_OrderBase.generated.h"

class ATCFSquadActor;
class UTCFOrderDefinition;
class UAbilitySystemComponent;
class UGameplayEffect;

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFGameplayAbility_OrderBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTCFGameplayAbility_OrderBase();

	UFUNCTION(BlueprintPure, Category = "TCF|Order Ability")
	FTCFSquadOrderRequest GetCurrentOrderRequest() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Order Ability")
	UTCFOrderDefinition* GetCurrentOrderDefinition() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Order Ability")
	ATCFSquadActor* GetSourceSquad() const;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	bool ApplyGameplayEffectClassToSelf(
		TSubclassOf<UGameplayEffect> EffectClass,
		float EffectLevel,
		UObject* SourceObject = nullptr) const;

	int32 ApplyGameplayEffectsToSelf(
		const TArray<TSubclassOf<UGameplayEffect>>& EffectClasses,
		float EffectLevel,
		UObject* SourceObject = nullptr) const;

	bool ApplyGameplayEffectClassToTarget(
		UAbilitySystemComponent* TargetAbilitySystem,
		TSubclassOf<UGameplayEffect> EffectClass,
		float EffectLevel,
		UObject* SourceObject = nullptr) const;

	int32 ApplyGameplayEffectsToTarget(
		UAbilitySystemComponent* TargetAbilitySystem,
		const TArray<TSubclassOf<UGameplayEffect>>& EffectClasses,
		float EffectLevel,
		UObject* SourceObject = nullptr) const;

	virtual void HandleOrderActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Order Ability")
	void ReceiveOrderActivated(const FTCFSquadOrderRequest& Request, UTCFOrderDefinition* OrderDefinition);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Order Ability")
	bool bCommitAbilityOnOrderActivated = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Order Ability")
	bool bEndAbilityAfterOrderActivated = true;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Order Ability")
	FTCFSquadOrderRequest CurrentOrderRequest;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Order Ability")
	TObjectPtr<UTCFOrderDefinition> CurrentOrderDefinition;

private:
	bool CaptureOrderPayload(const FGameplayEventData* TriggerEventData);
	void ClearOrderData();
};
