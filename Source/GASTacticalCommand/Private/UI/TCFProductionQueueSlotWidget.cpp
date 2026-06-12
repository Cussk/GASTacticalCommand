//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFProductionQueueSlotWidget.h"

#include "Components/ProgressBar.h"
#include "UI/TCFProductionQueueTooltipWidget.h"

void UTCFProductionQueueSlotWidget::SetQueueItemViewData(
	const FTCFProductionQueueItemViewData& InViewData,
	int32 InSlotIndex)
{
	ViewData = InViewData;
	SlotIndex = InSlotIndex;
	bHasQueueItem = true;

	BP_OnQueueSlotDataChanged();
}

void UTCFProductionQueueSlotWidget::SetEmptyQueueSlot(int32 InSlotIndex)
{
	ViewData = FTCFProductionQueueItemViewData();
	SlotIndex = InSlotIndex;
	bHasQueueItem = false;

	BP_OnQueueSlotDataChanged();
}

void UTCFProductionQueueSlotWidget::SetProgressBarStyle(const FProgressBarStyle& InStyle)
{
	QueueProgressBar->SetWidgetStyle(InStyle);
}

const FProgressBarStyle& UTCFProductionQueueSlotWidget::GetProgressBarStyle()
{
	return QueueProgressBar->GetWidgetStyle();
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

UTCFTooltipWidget* UTCFProductionQueueSlotWidget::GetTooltipWidgetForSource()
{
	if (!bHasQueueItem)
	{
		return nullptr;
	}

	return GetOrCreateQueueTooltipWidget();
}

UTCFProductionQueueTooltipWidget* UTCFProductionQueueSlotWidget::GetOrCreateQueueTooltipWidget()
{
	if (!QueueTooltipWidgetClass)
	{
		return nullptr;
	}

	if (!QueueTooltipWidget)
	{
		QueueTooltipWidget = CreateWidget<UTCFProductionQueueTooltipWidget>(
			GetOwningPlayer(),
			QueueTooltipWidgetClass);
	}

	if (QueueTooltipWidget)
	{
		QueueTooltipWidget->SetQueueItemViewData(ViewData);
	}

	return QueueTooltipWidget;
}

void UTCFProductionQueueSlotWidget::ClearTooltipData()
{
	if (QueueTooltipWidget)
	{
		QueueTooltipWidget->ClearQueueItemViewData();
	}
}

void UTCFProductionQueueSlotWidget::NativeOnReleasedToPool()
{
	Super::NativeOnReleasedToPool();

	ClearTooltipData();

	ViewData = FTCFProductionQueueItemViewData();
	bHasQueueItem = false;
	SlotIndex = INDEX_NONE;
}
