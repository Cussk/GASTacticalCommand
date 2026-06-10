// Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TCFGameplayAbility_ConstructBuilding.generated.h"

class ATCFPlayerController;
class ATCFBuildingActor;
class UTCFPlayerConstructionComponent;

UCLASS()
class GASTACTICALCOMMAND_API UTCFGameplayAbility_ConstructBuilding : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTCFGameplayAbility_ConstructBuilding();

protected:
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual bool CheckCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ApplyCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

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

private:
	mutable bool bResourceCostCommitted = false;

	UTCFPlayerConstructionComponent* ResolveConstructionComponent(
		const FGameplayAbilityActorInfo* ActorInfo) const;

	ATCFPlayerController* ResolvePlayerController(
		const FGameplayAbilityActorInfo* ActorInfo) const;

	void StartSelectedWorkersBuildingPlacedBuilding(
		const FGameplayAbilityActorInfo* ActorInfo,
		ATCFBuildingActor* PlacedBuilding) const;
};