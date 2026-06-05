//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Data/TCFProductionOptionDefinition.h"

#include "Actors/TCFSquadActor.h"
#include "Data/TCFSquadDefinition.h"

bool UTCFProductionOptionDefinition::IsValidProductionOption() const
{
	return IsValid(SquadDefinition) && SquadDefinition->IsValidDefinition();
}

UTCFSquadDefinition* UTCFProductionOptionDefinition::GetSquadDefinition() const
{
	return SquadDefinition;
}

TSubclassOf<ATCFSquadActor> UTCFProductionOptionDefinition::GetResolvedSquadActorClass(
	TSubclassOf<ATCFSquadActor> FallbackClass) const
{
	return SquadActorClass ? SquadActorClass : FallbackClass;
}

float UTCFProductionOptionDefinition::GetSafeProductionTime() const
{
	return FMath::Max(0.0f, ProductionTime);
}

FText UTCFProductionOptionDefinition::GetSafeDisplayName() const
{
	if (!DisplayName.IsEmpty())
	{
		return DisplayName;
	}

	return SquadDefinition ? SquadDefinition->DisplayName : FText::GetEmpty();
}