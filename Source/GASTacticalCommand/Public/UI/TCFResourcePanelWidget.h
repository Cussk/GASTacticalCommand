//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "TCFResourcePanelWidget.generated.h"

class ATCFPlayerState;
class UPanelWidget;
class UTCFTooltipSourceWidget;
class UTCFTooltipWidget;
class UTCFPlayerResourceBankComponent;
class UTCFPlayerUISubsystem;
class UTCFResourceAmountWidget;
class UTCFResourceUIDefinition;

UCLASS()
class GASTACTICALCOMMAND_API UTCFResourcePanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Resources|UI")
	void SetPlayerUISubsystem(UTCFPlayerUISubsystem* InPlayerUISubsystem);

	UFUNCTION(BlueprintCallable, Category = "TCF|Resources|UI")
	void RefreshResourcePanel();
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Resources|UI")
	void RefreshResourceAmount(FGameplayTag ResourceType);

	UFUNCTION(BlueprintPure, Category = "TCF|Resources|UI")
	UTCFPlayerResourceBankComponent* GetObservedResourceBank() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resources|UI")
	TSubclassOf<UTCFResourceAmountWidget> ResourceAmountWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|UI", meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ResourceListContainer;

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Resources|UI")
	void BP_OnResourcePanelRefreshed();

private:
	UPROPERTY()
	TObjectPtr<UTCFPlayerUISubsystem> PlayerUISubsystem;

	UPROPERTY()
	TObjectPtr<UTCFPlayerResourceBankComponent> ObservedResourceBank;

	UPROPERTY()
	TArray<TObjectPtr<UTCFResourceAmountWidget>> ResourceAmountWidgets;
	
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UTCFResourceAmountWidget>> ResourceAmountWidgetsByTag;

	UFUNCTION()
	void HandleResourceAmountChanged(
		FGameplayTag ResourceType,
		int32 OldAmount,
		int32 NewAmount);

	void BindObservedResourceBank();
	void UnbindObservedResourceBank();

	ATCFPlayerState* ResolvePlayerState() const;
	UTCFResourceUIDefinition* ResolveResourceUIDefinition() const;

	void RefreshObservedResourceBank();
	void EnsureResourceAmountWidgetCount(int32 RequiredCount);
	void ReleaseResourceAmountWidgetsFromIndex(int32 FirstIndexToRelease);

	UTCFResourceAmountWidget* CreateResourceAmountWidget();
	
	UFUNCTION()
	void HandleTooltipRequested(UTCFTooltipSourceWidget* SourceWidget, UTCFTooltipWidget* TCFTooltipWidget);

	UFUNCTION()
	void HandleTooltipCleared(UTCFTooltipSourceWidget* SourceWidget);

	void BindResourceAmountWidget(UTCFResourceAmountWidget* ResourceWidget);
	void UnbindResourceAmountWidget(UTCFResourceAmountWidget* ResourceWidget);
};