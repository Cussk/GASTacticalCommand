//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TCFBuildOptionButtonWidget.generated.h"

class UTCFConstructionOptionDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnTCFBuildOptionClicked,
	UTCFConstructionOptionDefinition*,
	ConstructionOption);

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFBuildOptionButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Build")
	void SetConstructionOption(UTCFConstructionOptionDefinition* InConstructionOption);

	UFUNCTION(BlueprintPure, Category = "TCF|Build")
	UTCFConstructionOptionDefinition* GetConstructionOption() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Build")
	void NotifyBuildOptionClicked();

	UPROPERTY(BlueprintAssignable, Category = "TCF|Build")
	FOnTCFBuildOptionClicked OnBuildOptionClicked;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Build")
	void BP_OnConstructionOptionChanged();

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTCFConstructionOptionDefinition> ConstructionOption;
};