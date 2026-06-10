//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "TCFWorkerPanelWidget.generated.h"

class ATCFPlayerController;
class ATCFSquadActor;
class UTCFPlayerSelectionComponent;
class UTCFPlayerUISubsystem;

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFWorkerPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Player UI")
	void SetPlayerUISubsystem(UTCFPlayerUISubsystem* InPlayerUISubsystem);

	UFUNCTION(BlueprintPure, Category = "TCF|Player UI")
	UTCFPlayerUISubsystem* GetPlayerUISubsystem() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Worker")
	void RefreshWorkerPanel();

	UFUNCTION(BlueprintCallable, Category = "TCF|Worker")
	void RequestBuildMenu();

	UFUNCTION(BlueprintCallable, Category = "TCF|Worker")
	void RequestStopCommands();

	UFUNCTION(BlueprintPure, Category = "TCF|Worker")
	int32 GetSelectedWorkerCount() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Worker")
	int32 GetSelectedNonWorkerCount() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Worker")
	bool HasSelectedWorkers() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Worker")
	bool HasOnlySelectedWorkers() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Worker")
	FGameplayTag WorkerRoleTag;

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Worker")
	void BP_OnWorkerPanelDataChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Worker")
	void BP_OnBuildMenuRequested();

private:
	UPROPERTY()
	TObjectPtr<UTCFPlayerUISubsystem> PlayerUISubsystem;

	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> ObservedSelectionComponent;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Worker", meta = (AllowPrivateAccess = true))
	int32 SelectedWorkerCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Worker", meta = (AllowPrivateAccess = true))
	int32 SelectedNonWorkerCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Worker", meta = (AllowPrivateAccess = true))
	bool bHasSelectedWorkers = false;

	UFUNCTION()
	void HandleSelectionCountChanged(int32 SelectedCount);

	UFUNCTION()
	void HandlePrimarySelectedSquadChanged(ATCFSquadActor* SelectedSquad);

	void BindSelectionComponent();
	void UnbindSelectionComponent();

	ATCFPlayerController* ResolveTCFPlayerController() const;
	UTCFPlayerSelectionComponent* ResolveSelectionComponent() const;

	bool IsWorkerSquad(const ATCFSquadActor* Squad) const;
};