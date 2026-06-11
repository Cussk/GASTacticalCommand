//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TCFRTSHUDWidget.generated.h"

class UTCFWorkerPanelWidget;
class UTCFResourcePanelWidget;
class UTCFPlayerUISubsystem;
class UTCFResourceUIDefinition;
class UTCFTooltipWidget;
class UTCFProductionPanelWidget;
class UTCFTooltipSourceWidget;
class UCanvasPanel;
class UWidget;

UCLASS()
class GASTACTICALCOMMAND_API UTCFRTSHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI")
	void SetPlayerUISubsystem(UTCFPlayerUISubsystem* InPlayerUISubsystem);

	UFUNCTION(BlueprintPure, Category = "TCF|Player UI")
	UTCFPlayerUISubsystem* GetPlayerUISubsystem() const;
	
	UFUNCTION(BlueprintPure, Category = "TCF|HUD")
	UTCFProductionPanelWidget* GetProductionPanel() const;
	
	UFUNCTION(BlueprintPure, Category = "TCF|HUD")
	UTCFWorkerPanelWidget* GetWorkerPanel() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|HUD")
	void RefreshHUDPanelVisibility();
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Tooltip")
	void RequestTooltip(UTCFTooltipSourceWidget* SourceWidget, UTCFTooltipWidget* TCFTooltipWidget);

	UFUNCTION(BlueprintCallable, Category = "TCF|Tooltip")
	void ClearTooltip(UTCFTooltipSourceWidget* SourceWidget);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Tooltip", meta = (BindWidgetOptional))
	TObjectPtr<UCanvasPanel> TooltipLayer;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|UI", meta = (BindWidgetOptional))
	TObjectPtr<UTCFResourcePanelWidget> ResourcePanel;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|HUD", meta = (BindWidgetOptional))
	TObjectPtr<UTCFProductionPanelWidget> ProductionPanel;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Worker", meta = (BindWidgetOptional))
	TObjectPtr<UTCFWorkerPanelWidget> WorkerPanel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tooltip")
	float TooltipShowDelaySeconds = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tooltip")
	float TooltipResetGraceSeconds = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tooltip")
	FVector2D TooltipCursorOffset = FVector2D(8.0f, 82.5f);

private:	
	UPROPERTY()
	TObjectPtr<UTCFPlayerUISubsystem> PlayerUISubsystem;
	
	UPROPERTY()
	TObjectPtr<UTCFTooltipWidget> ActiveTooltipWidget;

	UPROPERTY()
	TObjectPtr<UTCFTooltipWidget> PendingTooltipWidget;

	UPROPERTY()
	TObjectPtr<UTCFTooltipSourceWidget> ActiveTooltipSource;

	UPROPERTY()
	TObjectPtr<UTCFTooltipSourceWidget> PendingTooltipSource;

	FTimerHandle TooltipShowTimerHandle;
	FTimerHandle TooltipResetTimerHandle;
	
	UFUNCTION()
	void HandleProductionPanelDataChanged();
	
	void InitializeChildPanels();
	void UninitializeChildPanels();
	void RegisterGameplayInputBlockers() const;

	void ShowPendingTooltip();
	void ShowTooltipNow(UTCFTooltipSourceWidget* SourceWidget, UTCFTooltipWidget* TCFTooltipWidget);

	void HideActiveTooltip();
	void UpdateActiveTooltipPosition() const;
};