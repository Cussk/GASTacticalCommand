//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_ProduceSquad.h"

#include "Actors/TCFBuildingActor.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFBuildingProductionComponent.h"
#include "Components/TCFPlayerResourceBankComponent.h"
#include "Data/TCFProductionOptionDefinition.h"
#include "Types/TCFProductionTypes.h"

UTCFGameplayAbility_ProduceSquad::UTCFGameplayAbility_ProduceSquad()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

bool UTCFGameplayAbility_ProduceSquad::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UTCFBuildingProductionComponent* ProductionComponent = ResolveProductionComponent(ActorInfo);
	return ProductionComponent && ProductionComponent->CanExecutePendingProductionRequest();
}

bool UTCFGameplayAbility_ProduceSquad::CheckCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}

	const UTCFBuildingProductionComponent* ProductionComponent = ResolveProductionComponent(ActorInfo);
	if (!ProductionComponent || !ProductionComponent->HasPendingProductionRequest())
	{
		return false;
	}

	const FTCFProductionRequest& ProductionRequest = ProductionComponent->GetPendingProductionRequest();
	const UTCFProductionOptionDefinition* ProductionOption = ProductionRequest.ProductionOption;
	const UTCFPlayerResourceBankComponent* ResourceBank = ProductionComponent->GetResourceBankForPendingRequest();

	return ProductionOption
		&& ResourceBank
		&& ResourceBank->CanAffordResources(ProductionOption->Cost);
}

void UTCFGameplayAbility_ProduceSquad::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	bResourceCostCommitted = false;

	const UTCFBuildingProductionComponent* ProductionComponent = ResolveProductionComponent(ActorInfo);
	if (!ProductionComponent || !ProductionComponent->HasPendingProductionRequest())
	{
		return;
	}

	const FTCFProductionRequest& ProductionRequest = ProductionComponent->GetPendingProductionRequest();
	const UTCFProductionOptionDefinition* ProductionOption = ProductionRequest.ProductionOption;
	UTCFPlayerResourceBankComponent* ResourceBank = ProductionComponent->GetResourceBankForPendingRequest();

	if (!ProductionOption || !ResourceBank)
	{
		return;
	}

	FTCFResourceTransactionResult SpendResult;
	bResourceCostCommitted = ResourceBank->TrySpendResources(ProductionOption->Cost, SpendResult);
}

void UTCFGameplayAbility_ProduceSquad::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	bResourceCostCommitted = false;

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UTCFBuildingProductionComponent* ProductionComponent = ResolveProductionComponent(ActorInfo);
	if (!ProductionComponent || !bResourceCostCommitted)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!ProductionComponent->EnqueuePendingProduction())
	{
		ProductionComponent->RefundPendingProductionCost();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UTCFGameplayAbility_ProduceSquad::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (UTCFBuildingProductionComponent* ProductionComponent = ResolveProductionComponent(ActorInfo))
	{
		ProductionComponent->ClearPendingProductionRequest();
	}

	Super::EndAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		bReplicateEndAbility,
		bWasCancelled);
}

UTCFBuildingProductionComponent* UTCFGameplayAbility_ProduceSquad::ResolveProductionComponent(
	const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo)
	{
		return nullptr;
	}

	ATCFBuildingActor* Building = Cast<ATCFBuildingActor>(ActorInfo->OwnerActor.Get());
	if (!Building)
	{
		Building = Cast<ATCFBuildingActor>(ActorInfo->AvatarActor.Get());
	}

	return Building ? Building->GetProductionComponent() : nullptr;
}