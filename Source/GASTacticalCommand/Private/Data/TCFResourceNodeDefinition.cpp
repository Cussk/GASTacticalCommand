//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Data/TCFResourceNodeDefinition.h"

int32 UTCFResourceNodeDefinition::RollInitialAmount() const
{
	const int32 SafeMinimum = FMath::Max(0, MinimumInitialAmount);
	const int32 SafeMaximum = FMath::Max(SafeMinimum, MaximumInitialAmount);

	if (SafeMinimum == SafeMaximum)
	{
		return SafeMinimum;
	}

	return FMath::RandRange(SafeMinimum, SafeMaximum);
}

int32 UTCFResourceNodeDefinition::GetSafeGatherAmount() const
{
	return FMath::Max(1, GatherAmountPerAction);
}