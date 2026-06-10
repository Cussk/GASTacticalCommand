//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/TCFProductionTypes.h"
#include "Types/TCFProductionUIViewTypes.h"
#include "TCFProductionPanelWidget.generated.h"

class UTCFResourceUIDefinition;
class UTCFPlayerUISubsystem;
class UTCFRTSHUDWidget;
class UTCFTooltipWidget;
class UTCFTooltipSourceWidget;
class ATCFBuildingActor;
class ATCFPlayerState;
class UPanelWidget;
class UTCFBuildingProductionComponent;
class UTCFPlayerSelectionComponent;
class UTCFProductionOptionButtonWidget;
class UTCFProductionOptionDefinition;
class UTCFProductionQueueSlotWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTCFProductionPanelDataChanged);

UCLASS()
class GASTACTICALCOMMAND_API UTCFProductionPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI")
	void SetPlayerUISubsystem(UTCFPlayerUISubsystem* InPlayerUISubsystem);

	UFUNCTION(BlueprintPure, Category = "TCF|Player UI")
	UTCFPlayerUISubsystem* GetPlayerUISubsystem() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resources|UI")
	UTCFResourceUIDefinition* GetResourceUIDefinition() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void RefreshPanelData();

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void SetObservedBuilding(ATCFBuildingActor* Building);

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	bool RequestProductionOption(UTCFProductionOptionDefinition* ProductionOption);

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	ATCFBuildingActor* GetObservedBuilding() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	bool HasObservedProductionBuilding() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void GetProductionOptionViewData(TArray<FTCFProductionOptionViewData>& OutOptions) const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void GetProductionQueueViewData(TArray<FTCFProductionQueueItemViewData>& OutQueue) const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Production")
	FOnTCFProductionPanelDataChanged OnProductionPanelDataChanged;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|Widgets")
	TSubclassOf<UTCFProductionOptionButtonWidget> ProductionOptionButtonWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|Widgets")
	TSubclassOf<UTCFProductionQueueSlotWidget> ProductionQueueSlotWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production|Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ProductionOptionsContainer;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production|Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ProductionQueueContainer;

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Production")
	void BP_OnProductionPanelDataChanged();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Production")
	void BP_OnObservedBuildingChanged();

private:
	UPROPERTY()
	TObjectPtr<UTCFPlayerUISubsystem> PlayerUISubsystem;
	
	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> ObservedSelectionComponent;

	UPROPERTY()
	TObjectPtr<ATCFBuildingActor> ObservedBuilding;

	UPROPERTY()
	TObjectPtr<UTCFBuildingProductionComponent> ObservedProductionComponent;

	UPROPERTY()
	TArray<FTCFProductionOptionViewData> CachedProductionOptions;

	UPROPERTY()
	TArray<FTCFProductionQueueItemViewData> CachedProductionQueue;

	UPROPERTY()
	TArray<TObjectPtr<UTCFProductionOptionButtonWidget>> OptionButtonPool;

	UPROPERTY()
	TArray<TObjectPtr<UTCFProductionQueueSlotWidget>> QueueSlotPool;

	UFUNCTION()
	void HandleInspectedBuildingChanged(ATCFBuildingActor* InspectedBuilding);

	UFUNCTION()
	void HandleProductionQueueChanged(UTCFBuildingProductionComponent* ProductionComponent);

	UFUNCTION()
	void HandleProductionProgressChanged(
		UTCFBuildingProductionComponent* ProductionComponent,
		float ActiveProgressAlpha);

	UFUNCTION()
	void HandleProductionOptionButtonClicked(
		UTCFProductionOptionButtonWidget* ButtonWidget,
		UTCFProductionOptionDefinition* ProductionOption);

	void BindSelectionComponent();
	void UnbindSelectionComponent();

	void BindProductionComponent(UTCFBuildingProductionComponent* ProductionComponent);
	void UnbindProductionComponent();

	void RebuildProductionOptions();
	void RebuildProductionQueue();

	void RefreshOptionButtonPool();
	void RefreshQueueSlotPool();

	void EnsureOptionButtonPoolSize(int32 RequiredCount);
	void EnsureQueueSlotPoolSize(int32 RequiredCount);

	UTCFProductionOptionButtonWidget* CreateOptionButtonWidget();
	UTCFProductionQueueSlotWidget* CreateQueueSlotWidget() const;

	void ReleaseOptionButtonsFromIndex(int32 FirstIndexToRelease);
	void ReleaseQueueSlotsFromIndex(int32 FirstIndexToRelease);

	void UnbindPooledWidgetDelegates();

	UFUNCTION()
	void HandleTooltipRequested(UTCFTooltipSourceWidget* SourceWidget, UTCFTooltipWidget* TCFTooltipWidget);

	UFUNCTION()
	void HandleTooltipCleared(UTCFTooltipSourceWidget* SourceWidget);

	ATCFPlayerState* ResolveRequestingPlayerState() const;
};