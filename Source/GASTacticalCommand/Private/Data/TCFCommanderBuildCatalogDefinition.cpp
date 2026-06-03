// Copyright Kyle Cuss and Cuss Programming 2026.

#include "Data/TCFCommanderBuildCatalogDefinition.h"

#include "Data/TCFConstructionOptionDefinition.h"

void UTCFCommanderBuildCatalogDefinition::GetConstructionOptions(
	TArray<UTCFConstructionOptionDefinition*>& OutOptions) const
{
	OutOptions.Reset();

	for (UTCFConstructionOptionDefinition* Option : ConstructionOptions)
	{
		if (IsValid(Option))
		{
			OutOptions.Add(Option);
		}
	}
}