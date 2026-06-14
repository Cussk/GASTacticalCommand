//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFUIViewTypes.h"
#include "UI/TCFTooltipWidget.h"
#include "TCFActionTooltipWidget.generated.h"

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFActionTooltipWidget : public UTCFTooltipWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Tooltip")
	void SetActionViewData(const FTCFActionButtonViewData& InViewData);

	UFUNCTION(BlueprintCallable, Category = "TCF|Tooltip")
	void ClearActionViewData();

	UFUNCTION(BlueprintPure, Category = "TCF|Tooltip")
	const FTCFActionButtonViewData& GetActionViewData() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Tooltip")
	void BP_OnActionTooltipDataChanged();

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Tooltip", meta = (AllowPrivateAccess = true))
	FTCFActionButtonViewData ViewData;
};