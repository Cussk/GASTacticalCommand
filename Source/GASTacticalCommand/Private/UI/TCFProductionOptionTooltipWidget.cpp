//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFProductionOptionTooltipWidget.h"

void UTCFProductionOptionTooltipWidget::SetOptionViewData(
	const FTCFProductionOptionViewData& InViewData)
{
	ViewData = InViewData;
	SetTooltipHeader(ViewData.DisplayName, ViewData.Description);
	NotifyTooltipDataChanged();
}

void UTCFProductionOptionTooltipWidget::ClearOptionViewData()
{
	ViewData = FTCFProductionOptionViewData();
	ClearTooltipHeader();
	NotifyTooltipDataChanged();
}

const FTCFProductionOptionViewData& UTCFProductionOptionTooltipWidget::GetOptionViewData() const
{
	return ViewData;
}