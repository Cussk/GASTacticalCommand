//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFStandardTooltipWidget.h"

void UTCFStandardTooltipWidget::SetTooltipViewData(
	const FTCFTooltipViewData& InViewData)
{
	ViewData = InViewData;

	SetTooltipHeader(ViewData.Title, ViewData.Description);
	NotifyTooltipDataChanged();

	BP_OnTooltipViewDataChanged();
}

void UTCFStandardTooltipWidget::ClearTooltipViewData()
{
	ViewData = FTCFTooltipViewData();

	ClearTooltipHeader();
	NotifyTooltipDataChanged();

	BP_OnTooltipViewDataChanged();
}

const FTCFTooltipViewData& UTCFStandardTooltipWidget::GetTooltipViewData() const
{
	return ViewData;
}