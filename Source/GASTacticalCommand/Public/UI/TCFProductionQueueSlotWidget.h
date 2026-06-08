//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "UI/TCFPooledWidget.h"
#include "Types/TCFProductionUIViewTypes.h"
#include "TCFProductionQueueSlotWidget.generated.h"

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFProductionQueueSlotWidget : public UTCFPooledWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void SetQueueItemViewData(
		const FTCFProductionQueueItemViewData& InViewData,
		int32 InSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void SetEmptyQueueSlot(int32 InSlotIndex);

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	const FTCFProductionQueueItemViewData& GetQueueItemViewData() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	bool HasQueueItem() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	int32 GetSlotIndex() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Production")
	void BP_OnQueueSlotDataChanged();

	virtual void NativeOnReleasedToPool() override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production", meta = (AllowPrivateAccess = true))
	FTCFProductionQueueItemViewData ViewData;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production", meta = (AllowPrivateAccess = true))
	bool bHasQueueItem = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production", meta = (AllowPrivateAccess = true))
	int32 SlotIndex = INDEX_NONE;
};