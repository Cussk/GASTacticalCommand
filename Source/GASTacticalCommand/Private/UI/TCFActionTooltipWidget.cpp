//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFActionTooltipWidget.h"

void UTCFActionTooltipWidget::SetActionViewData(
	const FTCFActionButtonViewData& InViewData)
{
	ViewData = InViewData;

	SetTooltipHeader(ViewData.DisplayName, ViewData.Description);
	NotifyTooltipDataChanged();

	BP_OnActionTooltipDataChanged();
}

void UTCFActionTooltipWidget::ClearActionViewData()
{
	ViewData = FTCFActionButtonViewData();

	ClearTooltipHeader();
	NotifyTooltipDataChanged();

	BP_OnActionTooltipDataChanged();
}

const FTCFActionButtonViewData& UTCFActionTooltipWidget::GetActionViewData() const
{
	return ViewData;
}