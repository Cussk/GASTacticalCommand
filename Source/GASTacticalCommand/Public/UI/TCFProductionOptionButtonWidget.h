//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "UI/TCFTooltipSourceWidget.h"
#include "Types/TCFProductionUIViewTypes.h"
#include "TCFProductionOptionButtonWidget.generated.h"

class UTCFProductionOptionDefinition;
class UTCFProductionOptionButtonWidget;
class UTCFProductionOptionTooltipWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFProductionOptionButtonClicked,
	UTCFProductionOptionButtonWidget*,
	ButtonWidget,
	UTCFProductionOptionDefinition*,
	ProductionOption);

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFProductionOptionButtonWidget : public UTCFTooltipSourceWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void SetOptionViewData(const FTCFProductionOptionViewData& InViewData);

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	const FTCFProductionOptionViewData& GetOptionViewData() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	UTCFProductionOptionDefinition* GetProductionOption() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	bool NotifyProductionOptionClicked();

	UPROPERTY(BlueprintAssignable, Category = "TCF|Production")
	FOnTCFProductionOptionButtonClicked OnProductionOptionButtonClicked;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|Tooltip")
	TSubclassOf<UTCFProductionOptionTooltipWidget> OptionTooltipWidgetClass;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Production")
	void BP_OnOptionViewDataChanged();

	virtual void NativeOnReleasedToPool() override;
	
	virtual UTCFTooltipWidget* GetTooltipWidgetForSource() override;
	

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production", meta = (AllowPrivateAccess = true))
	FTCFProductionOptionViewData ViewData;
	
	UPROPERTY()
	TObjectPtr<UTCFProductionOptionTooltipWidget> OptionTooltipWidget;

	UTCFProductionOptionTooltipWidget* GetOrCreateOptionTooltipWidget();
	void ClearTooltipData() const;
};