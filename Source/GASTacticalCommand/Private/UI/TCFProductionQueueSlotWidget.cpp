//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFProductionQueueSlotWidget.h"

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

void UTCFProductionQueueSlotWidget::NativeOnReleasedToPool()
{
	Super::NativeOnReleasedToPool();

	ViewData = FTCFProductionQueueItemViewData();
	bHasQueueItem = false;
	SlotIndex = INDEX_NONE;
}