//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TCFRTSHUDWidget.generated.h"

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
	UFUNCTION(BlueprintPure, Category = "TCF|HUD")
	UTCFProductionPanelWidget* GetProductionPanel() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|HUD")
	void RefreshHUDPanelVisibility();
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Tooltip")
	void RequestTooltip(UTCFTooltipSourceWidget* SourceWidget, UTCFTooltipWidget* TCFTooltipWidget);

	UFUNCTION(BlueprintCallable, Category = "TCF|Tooltip")
	void ClearTooltip(UTCFTooltipSourceWidget* SourceWidget);

	UFUNCTION(BlueprintPure, Category = "TCF|HUD")
	bool IsMouseOverBlockingUI() const;
	
	// Will be moved to PlayerUI subsystem later
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resources|UI")
	TObjectPtr<UTCFResourceUIDefinition> ResourceUIDefinition;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Tooltip", meta = (BindWidgetOptional))
	TObjectPtr<UCanvasPanel> TooltipLayer;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|HUD", meta = (BindWidgetOptional))
	TObjectPtr<UTCFProductionPanelWidget> ProductionPanel;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|HUD", meta = (BindWidgetOptional))
	TObjectPtr<UWidget> ProductionPanelHost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tooltip")
	float TooltipShowDelaySeconds = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tooltip")
	float TooltipResetGraceSeconds = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tooltip")
	FVector2D TooltipCursorOffset = FVector2D(8.0f, 82.5f);

private:	
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

	void ShowPendingTooltip();
	void ShowTooltipNow(UTCFTooltipSourceWidget* SourceWidget, UTCFTooltipWidget* TCFTooltipWidget);

	void HideActiveTooltip();
	void UpdateActiveTooltipPosition() const;
	bool IsScreenPositionOverWidget(const UWidget* Widget, const FVector2D& ScreenPosition) const;
};