//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFUIViewTypes.h"
#include "UI/TCFTooltipWidget.h"
#include "TCFResourceTooltipWidget.generated.h"

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFResourceTooltipWidget : public UTCFTooltipWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Resources|Tooltip")
	void SetResourceTooltipViewData(const FTCFResourceTooltipViewData& InViewData);

	UFUNCTION(BlueprintCallable, Category = "TCF|Resources|Tooltip")
	void ClearResourceTooltipViewData();

	UFUNCTION(BlueprintPure, Category = "TCF|Resources|Tooltip")
	const FTCFResourceTooltipViewData& GetResourceTooltipViewData() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Resources|Tooltip")
	void BP_OnResourceTooltipDataChanged();

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|Tooltip", meta = (AllowPrivateAccess = true))
	FTCFResourceTooltipViewData ViewData;
};