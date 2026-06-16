//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFResourceAmountWidget.h"

#include "UI/TCFResourceTooltipWidget.h"

void UTCFResourceAmountWidget::SetResourceAmountData(
	const FTCFResourceUIViewData& InResourceViewData,
	int32 InAmount)
{
	ResourceViewData = InResourceViewData;
	ResourceAmount = FMath::Max(0, InAmount);
	bHasResourceData = ResourceViewData.ResourceTag.IsValid();
	
	if (ResourceTooltipWidget)
	{
		ResourceTooltipWidget->SetResourceTooltipViewData(BuildResourceTooltipViewData());
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

FTCFResourceTooltipViewData UTCFResourceAmountWidget::BuildResourceTooltipViewData() const
{
	FTCFResourceTooltipViewData TooltipViewData;
	TooltipViewData.ResourceTag = ResourceViewData.ResourceTag;
	TooltipViewData.DisplayName = ResourceViewData.DisplayName;
	TooltipViewData.Description = ResourceViewData.Description;
	TooltipViewData.IconBrush = ResourceViewData.IconBrush;
	TooltipViewData.CurrentAmount = ResourceAmount;

	return TooltipViewData;
}

UTCFResourceTooltipWidget* UTCFResourceAmountWidget::GetOrCreateResourceTooltipWidget()
{
	if (!ResourceTooltipWidgetClass || !bHasResourceData)
	{
		return nullptr;
	}

	if (!ResourceTooltipWidget)
	{
		ResourceTooltipWidget = CreateWidget<UTCFResourceTooltipWidget>(
			GetOwningPlayer(),
			ResourceTooltipWidgetClass);
	}

	if (ResourceTooltipWidget)
	{
		ResourceTooltipWidget->SetResourceTooltipViewData(BuildResourceTooltipViewData());
	}

	return ResourceTooltipWidget;
}

void UTCFResourceAmountWidget::ClearTooltipData() const
{
	if (ResourceTooltipWidget)
	{
		ResourceTooltipWidget->ClearResourceTooltipViewData();
	}
}