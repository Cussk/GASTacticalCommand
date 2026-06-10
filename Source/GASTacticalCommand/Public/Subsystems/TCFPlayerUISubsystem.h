//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "TCFPlayerUISubsystem.generated.h"

class UWidget;
class UTCFWorkerPanelWidget;
class UTCFProductionPanelWidget;
class APlayerController;
class UTCFResourceUIDefinition;
class UTCFRTSHUDWidget;
class UTCFTooltipSourceWidget;
class UTCFTooltipWidget;

UCLASS()
class GASTACTICALCOMMAND_API UTCFPlayerUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI")
	UTCFRTSHUDWidget* CreateRTSHUD(APlayerController* OwningPlayerController, TSubclassOf<UTCFRTSHUDWidget> RTSHUDWidgetClass);

	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI")
	void RegisterRTSHUD(UTCFRTSHUDWidget* InRTSHUDWidget);

	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI")
	void DestroyRTSHUD();

	UFUNCTION(BlueprintPure, Category = "TCF|Player UI")
	UTCFRTSHUDWidget* GetRTSHUD() const;
	
	UFUNCTION(BlueprintPure, Category = "TCF|Player UI|Panels")
	UTCFProductionPanelWidget* GetProductionPanel() const;
	
	UFUNCTION(BlueprintPure, Category = "TCF|Player UI|Panels")
	UTCFWorkerPanelWidget* GetWorkerPanel() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Player UI")
	bool HasRTSHUD() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI")
	void SetResourceUIDefinition(UTCFResourceUIDefinition* InResourceUIDefinition);

	UFUNCTION(BlueprintPure, Category = "TCF|Player UI")
	UTCFResourceUIDefinition* GetResourceUIDefinition() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI|Tooltip")
	void RequestTooltip(UTCFTooltipSourceWidget* SourceWidget, UTCFTooltipWidget* TooltipWidget);

	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI|Tooltip")
	void ClearTooltip(UTCFTooltipSourceWidget* SourceWidget);
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI|Input")
	void RegisterGameplayInputBlocker(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI|Input")
	void UnregisterGameplayInputBlocker(UWidget* Widget);

	UFUNCTION(BlueprintPure, Category = "TCF|Player UI")
	bool IsCursorOverBlockingUI() const;
	

private:
	UPROPERTY()
	TObjectPtr<UTCFRTSHUDWidget> RTSHUDWidget;

	UPROPERTY()
	TObjectPtr<UTCFResourceUIDefinition> ResourceUIDefinition;
	
	UPROPERTY()
	TArray<TObjectPtr<UWidget>> GameplayInputBlockers;
	
	bool IsScreenPositionOverWidget(const UWidget* Widget, const FVector2D& ScreenPosition) const;
};