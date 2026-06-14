// Copyright Kyle Cuss and Cuss Programming 2026.

#include "Data/TCFConstructionOptionDefinition.h"

#include "Data/TCFBuildingDefinition.h"

UTCFBuildingDefinition* UTCFConstructionOptionDefinition::GetBuildingDefinition() const
{
	return BuildingDefinition;
}

const TArray<FTCFResourceAmount>& UTCFConstructionOptionDefinition::GetEffectiveCost() const
{
	if (!ConstructionCost.IsEmpty())
	{
		return ConstructionCost;
	}

	static const TArray<FTCFResourceAmount> EmptyCost;
	return EmptyCost;
}

FText UTCFConstructionOptionDefinition::GetSafeDisplayName() const
{
	if (!DisplayName.IsEmpty())
	{
		return DisplayName;
	}

	return BuildingDefinition ? BuildingDefinition->DisplayName : FText::GetEmpty();
}

FText UTCFConstructionOptionDefinition::GetDescription() const
{
	if (!Description.IsEmpty())
	{
		return Description;
	}

	return BuildingDefinition ? BuildingDefinition->Description : FText::GetEmpty();
}

float UTCFConstructionOptionDefinition::GetRequiredConstructionWork() const
{
	return BuildingDefinition ? BuildingDefinition->GetSafeRequiredConstructionWork() : 0.0f;
}

