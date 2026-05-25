//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Debug/TCFDebugTypes.h"
#include "Types/TCFAffiliationTypes.h"
#include "TCFDebugSubsystem.generated.h"

enum class ETCFCapturePointState : uint8;
class ATCFSquadActor;
class UAbilitySystemComponent;
class UTCFDebugHUDWidget;
class UTCFPlayerOrderComponent;
class UTCFPlayerSelectionComponent;

UCLASS()
class GASTACTICALCOMMANDDEBUG_API UTCFDebugSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Debug")
	void RegisterPlayerController(APlayerController* PlayerController, float InRefreshInterval);

	UFUNCTION(BlueprintCallable, Category = "TCF|Debug")
	void UnregisterPlayerController(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "TCF|Debug")
	void ShowDebugHUD();

	UFUNCTION(BlueprintCallable, Category = "TCF|Debug")
	void HideDebugHUD();

	UFUNCTION(BlueprintCallable, Category = "TCF|Debug")
	void ToggleDebugHUD();

	UFUNCTION(BlueprintCallable, Category = "TCF|Debug")
	void RefreshDebugSnapshot();

	UFUNCTION(BlueprintPure, Category = "TCF|Debug")
	const FTCFDebugSquadSnapshot& GetCurrentSnapshot() const;

private:
	UPROPERTY()
	TWeakObjectPtr<APlayerController> ObservedPlayerController;

	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> ObservedSelectionComponent;

	UPROPERTY()
	TObjectPtr<UTCFPlayerOrderComponent> ObservedOrderComponent;

	UPROPERTY()
	TObjectPtr<UTCFDebugHUDWidget> DebugWidget;

	UPROPERTY()
	FTCFDebugSquadSnapshot CurrentSnapshot;

	FTimerHandle RefreshTimerHandle;

	float RefreshInterval = 0.20f;

	UFUNCTION()
	void HandleSelectedSquadChanged(ATCFSquadActor* SelectedSquad);

	UFUNCTION()
	void HandleOrderSubmitted(FTCFSquadOrderRequest Request, FTCFOrderResult Result);

	void BindObservedComponents();
	void UnbindObservedComponents();

	void StartRefreshTimer();
	void StopRefreshTimer();

	FTCFDebugSquadSnapshot BuildSnapshot() const;
	FTCFDebugOrderSnapshot BuildOrderSnapshot(const FTCFSquadOrderRequest& Request, const FTCFOrderResult& Result) const;

	static FString BuildTargetSummary(const FTCFOrderTarget& Target);
	static FString RelationshipToString(ETCFSquadRelationship Relationship);
	static FString CaptureStateToString(ETCFCapturePointState State);

	static void AddAffiliationData(const ATCFSquadActor& Squad, FTCFDebugSquadSnapshot& Snapshot);
	static void AddAttributeLines(const ATCFSquadActor& Squad, FTCFDebugSquadSnapshot& Snapshot);
	static void AddAbilityLines(const UAbilitySystemComponent& AbilitySystem, FTCFDebugSquadSnapshot& Snapshot);
	void AddActiveEffectLines(const UAbilitySystemComponent& AbilitySystem, FTCFDebugSquadSnapshot& Snapshot) const;
	void AddRelationshipLines(const ATCFSquadActor& Squad, FTCFDebugSquadSnapshot& Snapshot) const;
	void AddNearestCapturePointData(const ATCFSquadActor& Squad, FTCFDebugSquadSnapshot& Snapshot) const;
};