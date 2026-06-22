//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "TCFTooltipSourceWidget.h"
#include "Data/TCFResourceUIDefinition.h"
#include "Types/TCFUIViewTypes.h"
#include "TCFResourceAmountWidget.generated.h"

class UTCFStandardTooltipWidget;

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFResourceAmountWidget : public UTCFTooltipSourceWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Resources|UI")
	void SetResourceAmountData(
		const FTCFResourceUIViewData& InResourceViewData,
		int32 InAmount);

	UFUNCTION(BlueprintCallable, Category = "TCF|Resources|UI")
	void ClearResourceAmountData();

	UFUNCTION(BlueprintPure, Category = "TCF|Resources|UI")
	const FTCFResourceUIViewData& GetResourceViewData() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resources|UI")
	FGameplayTag GetResourceTag() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resources|UI")
	int32 GetResourceAmount() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resources|UI")
	bool HasResourceData() const;

protected:
	virtual UTCFTooltipWidget* GetTooltipWidgetForSource() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tooltip")
	TSubclassOf<UTCFStandardTooltipWidget> ResourceTooltipWidgetClass;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Resources|UI")
	void BP_OnResourceAmountDataChanged();

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|UI", meta = (AllowPrivateAccess = true))
	FTCFResourceUIViewData ResourceViewData;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|UI", meta = (AllowPrivateAccess = true))
	int32 ResourceAmount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|UI", meta = (AllowPrivateAccess = true))
	bool bHasResourceData = false;
	
	UPROPERTY()
	TObjectPtr<UTCFStandardTooltipWidget> ResourceTooltipWidget;

	FTCFTooltipViewData BuildTooltipViewData() const;
	UTCFStandardTooltipWidget* GetOrCreateResourceTooltipWidget();
	void ClearTooltipData() const;
};