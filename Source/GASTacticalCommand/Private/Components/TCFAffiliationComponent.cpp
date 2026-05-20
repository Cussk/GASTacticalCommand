//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFAffiliationComponent.h"

UTCFAffiliationComponent::UTCFAffiliationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

const FTCFAffiliationData& UTCFAffiliationComponent::GetAffiliation() const
{
	return Affiliation;
}

FTCFAffiliationData UTCFAffiliationComponent::GetAffiliationForBlueprint() const
{
	return Affiliation;
}

void UTCFAffiliationComponent::SetAffiliation(const FTCFAffiliationData& NewAffiliation)
{
	Affiliation = NewAffiliation;
	Affiliation.OwnerId = FMath::Max(0, Affiliation.OwnerId);
	Affiliation.TeamId = FMath::Max(0, Affiliation.TeamId);

	BroadcastAffiliationChanged();
}

void UTCFAffiliationComponent::SetOwnerId(int32 NewOwnerId)
{
	Affiliation.OwnerId = FMath::Max(0, NewOwnerId);
	BroadcastAffiliationChanged();
}

void UTCFAffiliationComponent::SetTeamId(int32 NewTeamId)
{
	Affiliation.TeamId = FMath::Max(0, NewTeamId);
	BroadcastAffiliationChanged();
}

void UTCFAffiliationComponent::SetFactionTag(FGameplayTag NewFactionTag)
{
	Affiliation.FactionTag = NewFactionTag;
	BroadcastAffiliationChanged();
}

void UTCFAffiliationComponent::BroadcastAffiliationChanged()
{
	OnAffiliationChanged.Broadcast(Affiliation);
}