//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFBuildOptionTooltipWidget.h"

void UTCFBuildOptionTooltipWidget::SetBuildOptionViewData(
	const FTCFBuildOptionViewData& InViewData)
{
	ViewData = InViewData;

	SetTooltipHeader(ViewData.DisplayName, ViewData.Description);
	NotifyTooltipDataChanged();

	BP_OnBuildOptionTooltipDataChanged();
}

void UTCFBuildOptionTooltipWidget::ClearBuildOptionViewData()
{
	ViewData = FTCFBuildOptionViewData();

	ClearTooltipHeader();
	NotifyTooltipDataChanged();

	BP_OnBuildOptionTooltipDataChanged();
}

const FTCFBuildOptionViewData& UTCFBuildOptionTooltipWidget::GetBuildOptionViewData() const
{
	return ViewData;
}