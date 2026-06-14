//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFResourceTooltipWidget.h"

void UTCFResourceTooltipWidget::SetResourceTooltipViewData(
	const FTCFResourceTooltipViewData& InViewData)
{
	ViewData = InViewData;

	SetTooltipHeader(ViewData.DisplayName, ViewData.Description);
	NotifyTooltipDataChanged();

	BP_OnResourceTooltipDataChanged();
}

void UTCFResourceTooltipWidget::ClearResourceTooltipViewData()
{
	ViewData = FTCFResourceTooltipViewData();

	ClearTooltipHeader();
	NotifyTooltipDataChanged();

	BP_OnResourceTooltipDataChanged();
}

const FTCFResourceTooltipViewData& UTCFResourceTooltipWidget::GetResourceTooltipViewData() const
{
	return ViewData;
}