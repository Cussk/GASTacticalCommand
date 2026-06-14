//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFUIViewTypes.h"
#include "UI/TCFTooltipSourceWidget.h"
#include "TCFBuildOptionButtonWidget.generated.h"

class UTCFBuildOptionTooltipWidget;
class UTCFConstructionOptionDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnTCFBuildOptionClicked,
	UTCFConstructionOptionDefinition*,
	ConstructionOption);

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFBuildOptionButtonWidget : public UTCFTooltipSourceWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Build")
	void SetBuildOptionViewData(const FTCFBuildOptionViewData& InViewData);

	UFUNCTION(BlueprintPure, Category = "TCF|Build")
	const FTCFBuildOptionViewData& GetBuildOptionViewData() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Build")
	UTCFConstructionOptionDefinition* GetConstructionOption() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Build")
	void NotifyBuildOptionClicked();

	UPROPERTY(BlueprintAssignable, Category = "TCF|Build")
	FOnTCFBuildOptionClicked OnBuildOptionClicked;

protected:
	virtual void NativeOnReleasedToPool() override;
	virtual UTCFTooltipWidget* GetTooltipWidgetForSource() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tooltip")
	TSubclassOf<UTCFBuildOptionTooltipWidget> BuildOptionTooltipWidgetClass;

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Build")
	void BP_OnBuildOptionViewDataChanged();

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build", meta = (AllowPrivateAccess = true))
	FTCFBuildOptionViewData ViewData;

	UPROPERTY()
	TObjectPtr<UTCFBuildOptionTooltipWidget> BuildOptionTooltipWidget;

	UTCFBuildOptionTooltipWidget* GetOrCreateBuildOptionTooltipWidget();
	void ClearTooltipData() const;
};