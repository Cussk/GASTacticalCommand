//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFProductionQueueTooltipWidget.h"

void UTCFProductionQueueTooltipWidget::SetQueueItemViewData(
	const FTCFProductionQueueItemViewData& InViewData)
{
	ViewData = InViewData;
	SetTooltipHeader(ViewData.DisplayName, FText::GetEmpty());
	NotifyTooltipDataChanged();
}

void UTCFProductionQueueTooltipWidget::ClearQueueItemViewData()
{
	ViewData = FTCFProductionQueueItemViewData();
	ClearTooltipHeader();
	NotifyTooltipDataChanged();
}

const FTCFProductionQueueItemViewData& UTCFProductionQueueTooltipWidget::GetQueueItemViewData() const
{
	return ViewData;
}