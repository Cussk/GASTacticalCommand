//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFStandardTooltipWidget.h"

#include "TimerManager.h"

void UTCFStandardTooltipWidget::SetTooltipViewData(
	const FTCFTooltipViewData& InViewData)
{
	ViewData = InViewData;

	SetTooltipHeader(ViewData.Title, ViewData.Description);
	NotifyTooltipDataChanged();

	BP_OnTooltipViewDataChanged();

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DeferredDetailRevealTimerHandle,
			this,
			&UTCFStandardTooltipWidget::HandleDeferredDetailReveal,
			0.025f,
			false);
	}
}

void UTCFStandardTooltipWidget::ClearTooltipViewData()
{
	ViewData = FTCFTooltipViewData();

	ClearTooltipHeader();
	NotifyTooltipDataChanged();

	BP_OnTooltipViewDataChanged();
	
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DeferredDetailRevealTimerHandle);
	}
}

const FTCFTooltipViewData& UTCFStandardTooltipWidget::GetTooltipViewData() const
{
	return ViewData;
}

void UTCFStandardTooltipWidget::HandleDeferredDetailReveal()
{
	BP_OnTooltipViewDataReadyForDetails();
}
