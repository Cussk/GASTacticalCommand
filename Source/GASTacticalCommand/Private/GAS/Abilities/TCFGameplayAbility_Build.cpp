//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_Build.h"

#include "Actors/TCFBuildingActor.h"
#include "Actors/TCFSquadActor.h"
#include "GAS/TCFSquadAttributeSet.h"
#include "TCFGameplayTags.h"

UTCFGameplayAbility_Build::UTCFGameplayAbility_Build()
{
	bCommitAbilityOnOrderActivated = true;
	bEndAbilityAfterOrderActivated = true;
}

bool UTCFGameplayAbility_Build::CanActivateCurrentOrder() const
{
	const ATCFSquadActor* SourceSquad = GetSourceSquad();
	const ATCFBuildingActor* TargetBuilding = GetTargetBuilding();

	if (!IsValid(SourceSquad) || !IsValid(TargetBuilding))
	{
		return false;
	}

	if (bRequireWorkerRole && !IsValidWorker(*SourceSquad))
	{
		return false;
	}

	return TargetBuilding->CanReceiveConstructionWork();
}

void UTCFGameplayAbility_Build::HandleOrderActivated()
{
	ATCFSquadActor* SourceSquad = GetSourceSquad();
	ATCFBuildingActor* TargetBuilding = GetTargetBuilding();

	if (!IsValid(SourceSquad) || !IsValid(TargetBuilding))
	{
		return;
	}

	const float ConstructionWork = CalculateConstructionWork(*SourceSquad);
	if (ConstructionWork <= 0.0f)
	{
		return;
	}

	if (!TargetBuilding->AddConstructionWork(ConstructionWork, SourceSquad))
	{
		return;
	}

	Super::HandleOrderActivated();
}

ATCFBuildingActor* UTCFGameplayAbility_Build::GetTargetBuilding() const
{
	if (CurrentOrderRequest.Target.TargetType != ETCFOrderTargetType::Actor)
	{
		return nullptr;
	}

	return Cast<ATCFBuildingActor>(CurrentOrderRequest.Target.TargetActor);
}

bool UTCFGameplayAbility_Build::IsValidWorker(const ATCFSquadActor& SourceSquad) const
{
	return SourceSquad.GetRoleTag().MatchesTagExact(TCFGameplayTags::Squad_Role_Worker);
}

float UTCFGameplayAbility_Build::CalculateConstructionWork(const ATCFSquadActor& SourceSquad) const
{
	float BuildRate = 1.0f;

	if (const UTCFSquadAttributeSet* AttributeSet = SourceSquad.GetSquadAttributeSet())
	{
		BuildRate = FMath::Max(0.0f, AttributeSet->GetBuildRate());
	}

	return FMath::Max(MinimumConstructionWork, BaseConstructionWorkPerActivation * BuildRate);
}