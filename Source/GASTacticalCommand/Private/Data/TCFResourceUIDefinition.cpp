//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Data/TCFResourceUIDefinition.h"

bool UTCFResourceUIDefinition::TryGetResourceViewData(FGameplayTag ResourceTag, FTCFResourceUIViewData& OutViewData) const
{
	if (!ResourceTag.IsValid())
	{
		return false;
	}

	for (const FTCFResourceUIViewData& Entry : ResourceViewData)
	{
		if (Entry.ResourceTag.MatchesTagExact(ResourceTag))
		{
			OutViewData = Entry;
			return true;
		}
	}

	return false;
}

void UTCFResourceUIDefinition::GetAllResourceViewData(TArray<FTCFResourceUIViewData>& OutResourceViewData) const
{
	OutResourceViewData = ResourceViewData;
}

FText UTCFResourceUIDefinition::GetResourceDisplayName(FGameplayTag ResourceTag) const
{
	FTCFResourceUIViewData ViewData;
	return TryGetResourceViewData(ResourceTag, ViewData)
		? ViewData.DisplayName
		: FText::FromName(ResourceTag.GetTagName());
}

FSlateBrush UTCFResourceUIDefinition::GetResourceIconBrush(FGameplayTag ResourceTag) const
{
	FTCFResourceUIViewData ViewData;
	return TryGetResourceViewData(ResourceTag, ViewData)
		? ViewData.IconBrush
		: FSlateBrush();
}