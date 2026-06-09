//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFProductionQueueSlotWidget.h"

#include "UI/TCFProductionQueueTooltipWidget.h"

void UTCFProductionQueueSlotWidget::SetQueueItemViewData(
	const FTCFProductionQueueItemViewData& InViewData,
	int32 InSlotIndex)
{
	ViewData = InViewData;
	SlotIndex = InSlotIndex;
	bHasQueueItem = true;

	RefreshTooltip();

	BP_OnQueueSlotDataChanged();
}

void UTCFProductionQueueSlotWidget::SetEmptyQueueSlot(int32 InSlotIndex)
{
	ViewData = FTCFProductionQueueItemViewData();
	SlotIndex = InSlotIndex;
	bHasQueueItem = false;

	BP_OnQueueSlotDataChanged();
}

const FTCFProductionQueueItemViewData& UTCFProductionQueueSlotWidget::GetQueueItemViewData() const
{
	return ViewData;
}

bool UTCFProductionQueueSlotWidget::HasQueueItem() const
{
	return bHasQueueItem;
}

int32 UTCFProductionQueueSlotWidget::GetSlotIndex() const
{
	return SlotIndex;
}

float UTCFProductionQueueSlotWidget::GetRemainingProgressAlpha() const
{
	if (!bHasQueueItem)
	{
		return 1.0f;
	}

	return 1.0f - FMath::Clamp(ViewData.ProgressAlpha, 0.0f, 1.0f);
}

void UTCFProductionQueueSlotWidget::RefreshTooltip()
{
	if (!bHasQueueItem || !QueueTooltipWidgetClass)
	{
		ClearTooltip();
		return;
	}

	if (!QueueTooltipWidget)
	{
		QueueTooltipWidget = CreateWidget<UTCFProductionQueueTooltipWidget>(
			GetOwningPlayer(),
			QueueTooltipWidgetClass);
	}

	if (!QueueTooltipWidget)
	{
		ClearTooltip();
		return;
	}

	QueueTooltipWidget->SetQueueItemViewData(ViewData);
	SetToolTip(QueueTooltipWidget);
}

void UTCFProductionQueueSlotWidget::ClearTooltip()
{
	if (QueueTooltipWidget)
	{
		QueueTooltipWidget->ClearQueueItemViewData();
	}

	SetToolTip(nullptr);
}

void UTCFProductionQueueSlotWidget::NativeOnReleasedToPool()
{
	Super::NativeOnReleasedToPool();

	ClearTooltip();

	ViewData = FTCFProductionQueueItemViewData();
	bHasQueueItem = false;
	SlotIndex = INDEX_NONE;
}
