//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFUIViewTypes.h"
#include "UI/TCFTooltipWidget.h"
#include "TCFStandardTooltipWidget.generated.h"

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFStandardTooltipWidget : public UTCFTooltipWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Tooltip")
	void SetTooltipViewData(const FTCFTooltipViewData& InViewData);

	UFUNCTION(BlueprintCallable, Category = "TCF|Tooltip")
	void ClearTooltipViewData();

	UFUNCTION(BlueprintPure, Category = "TCF|Tooltip")
	const FTCFTooltipViewData& GetTooltipViewData() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Tooltip")
	void BP_OnTooltipViewDataChanged();

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Tooltip", meta = (AllowPrivateAccess = true))
	FTCFTooltipViewData ViewData;
};