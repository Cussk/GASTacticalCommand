//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFResourceAmountWidget.h"

void UTCFResourceAmountWidget::SetResourceAmountData(
	const FTCFResourceUIViewData& InResourceViewData,
	int32 InAmount)
{
	ResourceViewData = InResourceViewData;
	ResourceAmount = FMath::Max(0, InAmount);
	bHasResourceData = ResourceViewData.ResourceTag.IsValid();

	SetVisibility(bHasResourceData ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	BP_OnResourceAmountDataChanged();
}

void UTCFResourceAmountWidget::ClearResourceAmountData()
{
	ResourceViewData = FTCFResourceUIViewData();
	ResourceAmount = 0;
	bHasResourceData = false;

	SetVisibility(ESlateVisibility::Collapsed);

	BP_OnResourceAmountDataChanged();
}

const FTCFResourceUIViewData& UTCFResourceAmountWidget::GetResourceViewData() const
{
	return ResourceViewData;
}

FGameplayTag UTCFResourceAmountWidget::GetResourceTag() const
{
	return ResourceViewData.ResourceTag;
}

int32 UTCFResourceAmountWidget::GetResourceAmount() const
{
	return ResourceAmount;
}

bool UTCFResourceAmountWidget::HasResourceData() const
{
	return bHasResourceData;
}