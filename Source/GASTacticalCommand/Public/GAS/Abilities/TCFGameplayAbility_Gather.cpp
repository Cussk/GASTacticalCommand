//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_Gather.h"

#include "Actors/TCFResourceNodeActor.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerResourceBankComponent.h"
#include "Data/TCFResourceNodeDefinition.h"
#include "GAS/TCFSquadAttributeSet.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Player/TCFPlayerState.h"
#include "TCFGameplayTags.h"

UTCFGameplayAbility_Gather::UTCFGameplayAbility_Gather()
{
	bCommitAbilityOnOrderActivated = true;
	bEndAbilityAfterOrderActivated = true;
}

void UTCFGameplayAbility_Gather::HandleOrderActivated()
{
	ATCFSquadActor* SourceSquad = GetSourceSquad();
	ATCFResourceNodeActor* ResourceNode = GetTargetResourceNode();

	if (!IsValid(SourceSquad) || !IsValid(ResourceNode))
	{
		return;
	}

	if (bRequireWorkerRole && !IsValidWorker(*SourceSquad))
	{
		return;
	}

	if (!ResourceNode->CanGatherResource())
	{
		return;
	}

	UTCFPlayerResourceBankComponent* ResourceBank = ResolveResourceBankForSource(*SourceSquad);
	if (!ResourceBank)
	{
		return;
	}

	FTCFResourceAmount GatheredResource;
	if (!ResourceNode->TryGatherResource(CalculateGatherAmount(*SourceSquad, *ResourceNode), GatheredResource))
	{
		return;
	}

	if (!GatheredResource.ResourceType.IsValid() || GatheredResource.Amount <= 0)
	{
		return;
	}

	ResourceBank->AddResourceAmount(GatheredResource.ResourceType, GatheredResource.Amount);

	Super::HandleOrderActivated();
}

ATCFResourceNodeActor* UTCFGameplayAbility_Gather::GetTargetResourceNode() const
{
	if (CurrentOrderRequest.Target.TargetType != ETCFOrderTargetType::Actor)
	{
		return nullptr;
	}

	return Cast<ATCFResourceNodeActor>(CurrentOrderRequest.Target.TargetActor);
}

bool UTCFGameplayAbility_Gather::IsValidWorker(const ATCFSquadActor& SourceSquad) const
{
	return SourceSquad.GetRoleTag().MatchesTagExact(TCFGameplayTags::Squad_Role_Worker);
}

int32 UTCFGameplayAbility_Gather::CalculateGatherAmount(
	const ATCFSquadActor& SourceSquad,
	const ATCFResourceNodeActor& ResourceNode) const
{
	const UTCFResourceNodeDefinition* NodeDefinition = ResourceNode.GetResourceNodeDefinition();
	const int32 BaseGatherAmount = NodeDefinition
		? NodeDefinition->GetSafeGatherAmount()
		: MinimumGatherAmount;

	float GatherRate = 1.0f;
	if (const UTCFSquadAttributeSet* AttributeSet = SourceSquad.GetSquadAttributeSet())
	{
		GatherRate = FMath::Max(0.0f, AttributeSet->GetGatherRate());
	}

	return FMath::Max(MinimumGatherAmount, FMath::RoundToInt(BaseGatherAmount * GatherRate));
}

ATCFPlayerState* UTCFGameplayAbility_Gather::ResolvePlayerStateForSource(const ATCFSquadActor& SourceSquad) const
{
	if (const AController* OwnerController = Cast<AController>(SourceSquad.GetOwner()))
	{
		return Cast<ATCFPlayerState>(OwnerController->PlayerState);
	}

	if (const AController* InstigatorController = SourceSquad.GetInstigatorController())
	{
		return Cast<ATCFPlayerState>(InstigatorController->PlayerState);
	}

	// For prototyping, use Player ID once using Multiplayer
	if (const UWorld* World = GetWorld())
	{
		if (const APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			return Cast<ATCFPlayerState>(PlayerController->PlayerState);
		}
	}

	return nullptr;
}

UTCFPlayerResourceBankComponent* UTCFGameplayAbility_Gather::ResolveResourceBankForSource(const ATCFSquadActor& SourceSquad) const
{
	const ATCFPlayerState* PlayerState = ResolvePlayerStateForSource(SourceSquad);
	return PlayerState ? PlayerState->GetPlayerResourceBankComponent() : nullptr;
}