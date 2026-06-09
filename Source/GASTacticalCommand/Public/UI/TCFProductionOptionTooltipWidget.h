//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFProductionUIViewTypes.h"
#include "UI/TCFTooltipWidget.h"
#include "TCFProductionOptionTooltipWidget.generated.h"

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFProductionOptionTooltipWidget : public UTCFTooltipWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Production|Tooltip")
	void SetOptionViewData(const FTCFProductionOptionViewData& InViewData);

	UFUNCTION(BlueprintCallable, Category = "TCF|Production|Tooltip")
	void ClearOptionViewData();

	UFUNCTION(BlueprintPure, Category = "TCF|Production|Tooltip")
	const FTCFProductionOptionViewData& GetOptionViewData() const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production|Tooltip", meta = (AllowPrivateAccess = true))
	FTCFProductionOptionViewData ViewData;
};