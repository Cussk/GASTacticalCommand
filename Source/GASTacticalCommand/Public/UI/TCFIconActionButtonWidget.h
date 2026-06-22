//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFUIViewTypes.h"
#include "UI/TCFTooltipSourceWidget.h"
#include "TCFIconActionButtonWidget.generated.h"

class UButton;
class UTCFStandardTooltipWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnTCFIconActionClicked,
	FName,
	ActionId);

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFIconActionButtonWidget : public UTCFTooltipSourceWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Action")
	void SetActionViewData(const FTCFActionButtonViewData& InViewData);

	UFUNCTION(BlueprintPure, Category = "TCF|Action")
	const FTCFActionButtonViewData& GetActionViewData() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Action")
	void NotifyActionClicked();

	UPROPERTY(BlueprintAssignable, Category = "TCF|Action")
	FOnTCFIconActionClicked OnActionClicked;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnReleasedToPool() override;
	virtual UTCFTooltipWidget* GetTooltipWidgetForSource() override;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Action", meta = (BindWidgetOptional))
	TObjectPtr<UButton> ActionButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tooltip")
	TSubclassOf<UTCFStandardTooltipWidget> ActionTooltipWidgetClass;

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Action")
	void BP_OnActionViewDataChanged();

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Action", meta = (AllowPrivateAccess = true))
	FTCFActionButtonViewData ViewData;

	UPROPERTY()
	TObjectPtr<UTCFStandardTooltipWidget> ActionTooltipWidget;

	UFUNCTION()
	void HandleButtonClicked();

	UTCFStandardTooltipWidget* GetOrCreateActionTooltipWidget();
	FTCFTooltipViewData BuildTooltipViewData() const;
	void ClearTooltipData() const;
};