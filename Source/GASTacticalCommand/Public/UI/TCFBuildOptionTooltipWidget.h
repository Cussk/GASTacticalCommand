//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFUIViewTypes.h"
#include "UI/TCFTooltipWidget.h"
#include "TCFBuildOptionTooltipWidget.generated.h"

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFBuildOptionTooltipWidget : public UTCFTooltipWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Build|Tooltip")
	void SetBuildOptionViewData(const FTCFBuildOptionViewData& InViewData);

	UFUNCTION(BlueprintCallable, Category = "TCF|Build|Tooltip")
	void ClearBuildOptionViewData();

	UFUNCTION(BlueprintPure, Category = "TCF|Build|Tooltip")
	const FTCFBuildOptionViewData& GetBuildOptionViewData() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Build|Tooltip")
	void BP_OnBuildOptionTooltipDataChanged();

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build|Tooltip", meta = (AllowPrivateAccess = true))
	FTCFBuildOptionViewData ViewData;
};