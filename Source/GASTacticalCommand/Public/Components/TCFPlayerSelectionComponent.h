//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCFPlayerSelectionComponent.generated.h"

class ATCFBuildingActor;
class ATCFSquadActor;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFSelectedSquadChanged, ATCFSquadActor*, SelectedSquad);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTCFOnInspectedSquadChanged, ATCFSquadActor*, SelectedSquad);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFSelectionCountChanged, int32, SelectedCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFInspectedBuildingChanged,ATCFBuildingActor*,InspectedBuilding);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFPlayerSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFPlayerSelectionComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool TrySelectSquad(ATCFSquadActor* Squad);

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool AddSquadToSelection(ATCFSquadActor* Squad);

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool RemoveSquadFromSelection(ATCFSquadActor* Squad);

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool ToggleSquadSelection(ATCFSquadActor* Squad);

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	void SetSelectedSquads(const TArray<ATCFSquadActor*>& Squads, bool bAppendSelection = false);

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	void ClearSelection();

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	ATCFSquadActor* GetSelectedSquad() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	ATCFSquadActor* GetPrimarySelectedSquad() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	void GetSelectedSquads(TArray<ATCFSquadActor*>& OutSelectedSquads) const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	int32 GetSelectedSquadCount() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool HasSelectedSquads() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool TryInspectSquad(ATCFSquadActor* Squad);
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	void ClearInspectedSquad();

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	ATCFSquadActor* GetInspectedSquad() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool HasInspectedSquad() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool TryInspectBuilding(ATCFBuildingActor* Building);

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	void ClearInspectedBuilding();

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	ATCFBuildingActor* GetInspectedBuilding() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool HasInspectedBuilding() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	UAbilitySystemComponent* GetInspectedBuildingAbilitySystem() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	UAbilitySystemComponent* GetSelectedSquadAbilitySystem() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Selection")
	FOnTCFSelectedSquadChanged OnSelectedSquadChanged;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Selection")
	FOnTCFSelectionCountChanged OnSelectionCountChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "TCF|Selection")
	FOnTCFInspectedBuildingChanged OnInspectedBuildingChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "TCF|Selection")
	FTCFOnInspectedSquadChanged OnInspectedSquadChanged;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "TCF|Selection")
	TArray<TObjectPtr<ATCFSquadActor>> SelectedSquads;

	UPROPERTY(VisibleInstanceOnly, Category = "TCF|Selection")
	TObjectPtr<ATCFSquadActor> PrimarySelectedSquad;
	
	UPROPERTY(VisibleInstanceOnly, Category = "TCF|Selection")
	TObjectPtr<ATCFBuildingActor> InspectedBuilding;
	
	UPROPERTY(VisibleInstanceOnly, Category = "TCF|Selection")
	TObjectPtr<ATCFSquadActor> InspectedSquad;

	void AddSquadInternal(ATCFSquadActor* Squad);
	void RemoveSquadInternal(ATCFSquadActor* Squad);
	void CompactSelection();
	void SetPrimarySelectedSquad(ATCFSquadActor* Squad);
	void BroadcastSelectionChanged() const;
	
	bool IsOwnSquad(const ATCFSquadActor* Squad) const;

	static void SetSquadSelectedState(const ATCFSquadActor* Squad, bool bSelected);
	
	void SetInspectedSquad(ATCFSquadActor* Squad);
	
	void SetInspectedBuilding(ATCFBuildingActor* Building);
	void BindInspectedBuildingDestroyed(ATCFBuildingActor* Building);
	void UnbindInspectedBuildingDestroyed();

	UFUNCTION()
	void HandleInspectedBuildingDestroyed(AActor* DestroyedActor);
};