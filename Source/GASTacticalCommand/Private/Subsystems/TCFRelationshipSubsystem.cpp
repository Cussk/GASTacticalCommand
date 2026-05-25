//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Subsystems/TCFRelationshipSubsystem.h"

#include "Components/TCFAffiliationComponent.h"
#include "GameFramework/Actor.h"

ETCFSquadRelationship UTCFRelationshipSubsystem::GetActorRelationship(
	const AActor* SourceActor,
	const AActor* OtherActor) const
{
	if (!IsValid(SourceActor) || !IsValid(OtherActor))
	{
		return ETCFSquadRelationship::Neutral;
	}

	if (SourceActor == OtherActor)
	{
		return ETCFSquadRelationship::Own;
	}

	const UTCFAffiliationComponent* SourceAffiliationComponent = FindAffiliationComponent(SourceActor);
	const UTCFAffiliationComponent* OtherAffiliationComponent = FindAffiliationComponent(OtherActor);

	if (!SourceAffiliationComponent || !OtherAffiliationComponent)
	{
		return ETCFSquadRelationship::Neutral;
	}

	return GetAffiliationRelationship(
		SourceAffiliationComponent->GetAffiliation(),
		OtherAffiliationComponent->GetAffiliation());
}

ETCFSquadRelationship UTCFRelationshipSubsystem::GetAffiliationRelationship(
	const FTCFAffiliationData& SourceAffiliation,
	const FTCFAffiliationData& OtherAffiliation) const
{
	if (SourceAffiliation.HasOwner() && OtherAffiliation.HasOwner())
	{
		if (SourceAffiliation.OwnerId == OtherAffiliation.OwnerId)
		{
			return ETCFSquadRelationship::Own;
		}
	}

	if (SourceAffiliation.HasTeam() && OtherAffiliation.HasTeam())
	{
		if (SourceAffiliation.TeamId == OtherAffiliation.TeamId)
		{
			return ETCFSquadRelationship::Friendly;
		}

		return ETCFSquadRelationship::Enemy;
	}

	if (SourceAffiliation.HasFaction() && OtherAffiliation.HasFaction())
	{
		if (SourceAffiliation.FactionTag.MatchesTagExact(OtherAffiliation.FactionTag))
		{
			return ETCFSquadRelationship::Friendly;
		}

		return ETCFSquadRelationship::Enemy;
	}

	return ETCFSquadRelationship::Neutral;
}

const UTCFAffiliationComponent* UTCFRelationshipSubsystem::FindAffiliationComponent(const AActor* Actor)
{
	return IsValid(Actor)
		? Actor->FindComponentByClass<UTCFAffiliationComponent>()
		: nullptr;
}
