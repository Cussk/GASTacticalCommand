//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCFPlayerSelectionComponent.generated.h"

class ATCFSquadActor;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFSelectedSquadChanged, ATCFSquadActor*, SelectedSquad);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFPlayerSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFPlayerSelectionComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool TrySelectSquad(ATCFSquadActor* Squad);

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	void ClearSelection();

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	ATCFSquadActor* GetSelectedSquad() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	UAbilitySystemComponent* GetSelectedSquadAbilitySystem() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Selection")
	FOnTCFSelectedSquadChanged OnSelectedSquadChanged;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "TCF|Selection")
	TObjectPtr<ATCFSquadActor> SelectedSquad;

	void SetSquadSelectedState(ATCFSquadActor* Squad, bool bSelected) const;
};