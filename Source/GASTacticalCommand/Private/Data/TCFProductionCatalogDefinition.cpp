//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Data/TCFProductionCatalogDefinition.h"

#include "Data/TCFProductionOptionDefinition.h"

void UTCFProductionCatalogDefinition::GetProductionOptions(
	TArray<UTCFProductionOptionDefinition*>& OutOptions) const
{
	OutOptions.Reset();

	for (UTCFProductionOptionDefinition* Option : ProductionOptions)
	{
		if (IsValid(Option))
		{
			OutOptions.Add(Option);
		}
	}

	OutOptions.Sort([](
		const UTCFProductionOptionDefinition& Left,
		const UTCFProductionOptionDefinition& Right)
	{
		return Left.SortPriority < Right.SortPriority;
	});
}

bool UTCFProductionCatalogDefinition::ContainsProductionOption(
	const UTCFProductionOptionDefinition* ProductionOption) const
{
	if (!IsValid(ProductionOption))
	{
		return false;
	}

	return ProductionOptions.Contains(ProductionOption);
}