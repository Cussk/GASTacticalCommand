//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TCFRTSHUDWidget.generated.h"

class UTCFProductionPanelWidget;
class UWidget;

UCLASS()
class GASTACTICALCOMMAND_API UTCFRTSHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TCF|HUD")
	UTCFProductionPanelWidget* GetProductionPanel() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|HUD")
	void RefreshHUDPanelVisibility();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|HUD", meta = (BindWidgetOptional))
	TObjectPtr<UTCFProductionPanelWidget> ProductionPanel;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|HUD", meta = (BindWidgetOptional))
	TObjectPtr<UWidget> ProductionPanelHost;

private:
	UFUNCTION()
	void HandleProductionPanelDataChanged();
};