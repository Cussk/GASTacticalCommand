//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "UI/TCFTooltipSourceWidget.h"
#include "Types/TCFUIViewTypes.h"
#include "TCFProductionQueueSlotWidget.generated.h"

class UProgressBar;
class UTCFProductionQueueTooltipWidget;

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFProductionQueueSlotWidget : public UTCFTooltipSourceWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void SetQueueItemViewData(
		const FTCFProductionQueueItemViewData& InViewData,
		int32 InSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void SetEmptyQueueSlot(int32 InSlotIndex);
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void SetProgressBarStyle(const FProgressBarStyle& InStyle);
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	const FProgressBarStyle& GetProgressBarStyle();

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	const FTCFProductionQueueItemViewData& GetQueueItemViewData() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	bool HasQueueItem() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	int32 GetSlotIndex() const;
	
	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	float GetRemainingProgressAlpha() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production|Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> QueueProgressBar;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|Tooltip")
	TSubclassOf<UTCFProductionQueueTooltipWidget> QueueTooltipWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production")
	TObjectPtr<UTexture2D> EmptySlotIcon;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Production")
	void BP_OnQueueSlotDataChanged();

	virtual void NativeOnReleasedToPool() override;
	virtual UTCFTooltipWidget* GetTooltipWidgetForSource() override;	

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production", meta = (AllowPrivateAccess = true))
	FTCFProductionQueueItemViewData ViewData;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production", meta = (AllowPrivateAccess = true))
	bool bHasQueueItem = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production", meta = (AllowPrivateAccess = true))
	int32 SlotIndex = INDEX_NONE;
	
	UPROPERTY()
	TObjectPtr<UTCFProductionQueueTooltipWidget> QueueTooltipWidget;

	UTCFProductionQueueTooltipWidget* GetOrCreateQueueTooltipWidget();
	void ClearTooltipData();
};