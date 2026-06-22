//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFResourceAmountWidget.h"

#include "UI/TCFStandardTooltipWidget.h"

void UTCFResourceAmountWidget::SetResourceAmountData(
	const FTCFResourceUIViewData& InResourceViewData,
	int32 InAmount)
{
	ResourceViewData = InResourceViewData;
	ResourceAmount = FMath::Max(0, InAmount);
	bHasResourceData = ResourceViewData.ResourceTag.IsValid();

	if (ResourceTooltipWidget)
	{
		ResourceTooltipWidget->SetTooltipViewData(BuildTooltipViewData());
	}

	SetVisibility(bHasResourceData ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	BP_OnResourceAmountDataChanged();
}

void UTCFResourceAmountWidget::ClearResourceAmountData()
{
	ResourceViewData = FTCFResourceUIViewData();
	ResourceAmount = 0;
	bHasResourceData = false;
	
	ClearTooltipData();

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

UTCFTooltipWidget* UTCFResourceAmountWidget::GetTooltipWidgetForSource()
{
	return GetOrCreateResourceTooltipWidget();
}

FTCFTooltipViewData UTCFResourceAmountWidget::BuildTooltipViewData() const
{
	FTCFTooltipViewData TooltipData;
	TooltipData.Title = ResourceViewData.DisplayName;
	TooltipData.Description = ResourceViewData.Description;
	TooltipData.Icon = ResourceViewData.Icon;
	TooltipData.ResourceAmount = ResourceAmount;
	TooltipData.bHasResourceAmount = bHasResourceData;

	return TooltipData;
}

UTCFStandardTooltipWidget* UTCFResourceAmountWidget::GetOrCreateResourceTooltipWidget()
{
	if (!ResourceTooltipWidgetClass || !bHasResourceData)
	{
		return nullptr;
	}

	if (!ResourceTooltipWidget)
	{
		ResourceTooltipWidget = CreateWidget<UTCFStandardTooltipWidget>(
			GetOwningPlayer(),
			ResourceTooltipWidgetClass);
	}

	if (ResourceTooltipWidget)
	{
		ResourceTooltipWidget->SetTooltipViewData(BuildTooltipViewData());
	}

	return ResourceTooltipWidget;
}

void UTCFResourceAmountWidget::ClearTooltipData() const
{
	if (ResourceTooltipWidget)
	{
		ResourceTooltipWidget->ClearTooltipViewData();
	}
}