//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TCFBuildPanelWidget.generated.h"

class UPanelWidget;
class UTCFBuildOptionButtonWidget;
class UTCFCommanderBuildCatalogDefinition;
class UTCFConstructionOptionDefinition;
class UTCFPlayerUISubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTCFBuildPanelVisibilityChanged);

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFBuildPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI")
	void SetPlayerUISubsystem(UTCFPlayerUISubsystem* InPlayerUISubsystem);

	UFUNCTION(BlueprintCallable, Category = "TCF|Build")
	void OpenBuildPanel();

	UFUNCTION(BlueprintCallable, Category = "TCF|Build")
	void CloseBuildPanel();

	UFUNCTION(BlueprintCallable, Category = "TCF|Build")
	void ToggleBuildPanel();

	UFUNCTION(BlueprintPure, Category = "TCF|Build")
	bool IsBuildPanelOpen() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Build")
	void RefreshBuildOptions();

	UPROPERTY(BlueprintAssignable, Category = "TCF|Build")
	FOnTCFBuildPanelVisibilityChanged OnBuildPanelVisibilityChanged;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Build")
	TObjectPtr<UTCFCommanderBuildCatalogDefinition> ConstructionCatalog;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Build")
	TSubclassOf<UTCFBuildOptionButtonWidget> BuildOptionButtonClass;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build", meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> BuildOptionContainer;

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Build")
	void BP_OnBuildPanelDataChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Build")
	void BP_OnBuildOptionRequested(UTCFConstructionOptionDefinition* ConstructionOption);

private:
	UPROPERTY()
	TObjectPtr<UTCFPlayerUISubsystem> PlayerUISubsystem;

	UPROPERTY()
	TArray<TObjectPtr<UTCFBuildOptionButtonWidget>> BuildOptionButtons;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build", meta = (AllowPrivateAccess = true))
	bool bIsBuildPanelOpen = false;

	UFUNCTION()
	void HandleBuildOptionClicked(UTCFConstructionOptionDefinition* ConstructionOption);

	UTCFBuildOptionButtonWidget* CreateBuildOptionButton();
	void ReleaseBuildOptionButtons();
};