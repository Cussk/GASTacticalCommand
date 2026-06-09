//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFProductionUIViewTypes.h"
#include "UI/TCFTooltipWidget.h"
#include "TCFProductionQueueTooltipWidget.generated.h"

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFProductionQueueTooltipWidget : public UTCFTooltipWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Production|Tooltip")
	void SetQueueItemViewData(const FTCFProductionQueueItemViewData& InViewData);

	UFUNCTION(BlueprintCallable, Category = "TCF|Production|Tooltip")
	void ClearQueueItemViewData();

	UFUNCTION(BlueprintPure, Category = "TCF|Production|Tooltip")
	const FTCFProductionQueueItemViewData& GetQueueItemViewData() const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production|Tooltip", meta = (AllowPrivateAccess = true))
	FTCFProductionQueueItemViewData ViewData;
};