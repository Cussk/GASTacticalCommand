//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "UI/TCFPooledWidget.h"
#include "Types/TCFProductionUIViewTypes.h"
#include "TCFProductionOptionButtonWidget.generated.h"

class UTCFProductionOptionDefinition;
class UTCFProductionOptionButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFProductionOptionButtonClicked,
	UTCFProductionOptionButtonWidget*,
	ButtonWidget,
	UTCFProductionOptionDefinition*,
	ProductionOption);

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFProductionOptionButtonWidget : public UTCFPooledWidget
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
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Production")
	void BP_OnOptionViewDataChanged();

	virtual void NativeOnReleasedToPool() override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production", meta = (AllowPrivateAccess = true))
	FTCFProductionOptionViewData ViewData;
};