// Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_ConstructBuilding.h"

#include "Actors/TCFBuildingActor.h"
#include "Components/TCFPlayerConstructionComponent.h"
#include "Components/TCFPlayerResourceBankComponent.h"
#include "Components/TCFRTSCommandRouterComponent.h"
#include "Data/TCFConstructionOptionDefinition.h"
#include "GameFramework/Actor.h"
#include "Player/TCFPlayerController.h"
#include "Player/TCFPlayerState.h"
#include "Types/TCFBuildingConstructionTypes.h"

UTCFGameplayAbility_ConstructBuilding::UTCFGameplayAbility_ConstructBuilding()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

bool UTCFGameplayAbility_ConstructBuilding::CanActivateAbility(
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

	const UTCFPlayerConstructionComponent* ConstructionComponent = ResolveConstructionComponent(ActorInfo);
	if (!ConstructionComponent)
	{
		return false;
	}

	const FTCFBuildingConstructionRequest& ConstructionRequest = ConstructionComponent->GetPendingConstructionRequest();
	if (!ConstructionRequest.IsValid())
	{
		return false;
	}

	FTCFConstructionAccessResult AccessResult;
	if (!ConstructionComponent->CanAccessConstructionOption(ConstructionRequest.ConstructionOption, AccessResult))
	{
		return false;
	}

	return ConstructionComponent->CanExecutePendingConstructionRequest();
}

bool UTCFGameplayAbility_ConstructBuilding::CheckCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}

	const UTCFPlayerConstructionComponent* ConstructionComponent = ResolveConstructionComponent(ActorInfo);
	if (!ConstructionComponent || !ConstructionComponent->HasPendingConstructionRequest())
	{
		return false;
	}

	const UTCFPlayerResourceBankComponent* ResourceBank = ConstructionComponent->GetResourceBankComponent();
	if (!ResourceBank)
	{
		return false;
	}

	const FTCFBuildingConstructionRequest& ConstructionRequest = ConstructionComponent->GetPendingConstructionRequest();
	const UTCFConstructionOptionDefinition* ConstructionOptionDefinition = ConstructionRequest.ConstructionOption;

	return ConstructionOptionDefinition && ResourceBank->CanAffordResources(ConstructionOptionDefinition->GetEffectiveCost());
}

void UTCFGameplayAbility_ConstructBuilding::ApplyCost(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	bResourceCostCommitted = false;

	const UTCFPlayerConstructionComponent* ConstructionComponent = ResolveConstructionComponent(ActorInfo);
	if (!ConstructionComponent || !ConstructionComponent->HasPendingConstructionRequest())
	{
		return;
	}

	UTCFPlayerResourceBankComponent* ResourceBank = ConstructionComponent->GetResourceBankComponent();
	if (!ResourceBank)
	{
		return;
	}

	const FTCFBuildingConstructionRequest& ConstructionRequest = ConstructionComponent->GetPendingConstructionRequest();
	const UTCFConstructionOptionDefinition* ConstructionOptionDefinition = ConstructionRequest.ConstructionOption;
	if (!ConstructionOptionDefinition)
	{
		return;
	}

	FTCFResourceTransactionResult SpendResult;
	bResourceCostCommitted = ResourceBank->TrySpendResources(ConstructionOptionDefinition->GetEffectiveCost(), SpendResult);
}

void UTCFGameplayAbility_ConstructBuilding::ActivateAbility(
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

	UTCFPlayerConstructionComponent* ConstructionComponent = ResolveConstructionComponent(ActorInfo);
	if (!ConstructionComponent || !bResourceCostCommitted)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ATCFBuildingActor* PlacedBuilding = nullptr;
	if (!ConstructionComponent->ExecutePendingConstruction(PlacedBuilding))
	{
		ConstructionComponent->RefundPendingConstructionCost();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	StartSelectedWorkersBuildingPlacedBuilding(ActorInfo, PlacedBuilding);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UTCFGameplayAbility_ConstructBuilding::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (UTCFPlayerConstructionComponent* ConstructionComponent = ResolveConstructionComponent(ActorInfo))
	{
		ConstructionComponent->ClearPendingConstructionRequest();
	}

	Super::EndAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		bReplicateEndAbility,
		bWasCancelled);
}

UTCFPlayerConstructionComponent* UTCFGameplayAbility_ConstructBuilding::ResolveConstructionComponent(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->OwnerActor.IsValid())
	{
		return nullptr;
	}

	const ATCFPlayerState* TCFPlayerState = Cast<ATCFPlayerState>(ActorInfo->OwnerActor.Get());
	return TCFPlayerState
		? TCFPlayerState->GetPlayerConstructionComponent()
		: nullptr;
}

ATCFPlayerController* UTCFGameplayAbility_ConstructBuilding::ResolvePlayerController(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->OwnerActor.IsValid())
	{
		return nullptr;
	}

	const ATCFPlayerState* TCFPlayerState = Cast<ATCFPlayerState>(ActorInfo->OwnerActor.Get());
	if (!TCFPlayerState)
	{
		return nullptr;
	}

	return Cast<ATCFPlayerController>(TCFPlayerState->GetOwner());
}

void UTCFGameplayAbility_ConstructBuilding::StartSelectedWorkersBuildingPlacedBuilding(const FGameplayAbilityActorInfo* ActorInfo, ATCFBuildingActor* PlacedBuilding) const
{
	if (!IsValid(PlacedBuilding) || !PlacedBuilding->CanReceiveConstructionWork())
	{
		return;
	}

	const ATCFPlayerController* PlayerController = ResolvePlayerController(ActorInfo);
	if (!PlayerController)
	{
		return;
	}

	UTCFRTSCommandRouterComponent* CommandRouterComponent = PlayerController->GetRTSCommandRouterComponent();

	if (!CommandRouterComponent)
	{
		return;
	}

	CommandRouterComponent->StartSelectedBuildCommandsForBuilding(PlacedBuilding);
}