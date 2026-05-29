//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/TCFOrderTypes.h"
#include "TCFPlayerOrderSubmissionComponent.generated.h"

class ATCFSquadActor;
class UTCFOrderSubsystem;
class UTCFPlayerSelectionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTCFPlayerOrderSubmitted, FTCFSquadOrderRequest, Request, FTCFOrderResult, Result);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFPlayerOrderSubmissionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFPlayerOrderSubmissionComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Orders")
	bool SubmitSelectedSquadOrder(FGameplayTag OrderTag, const FTCFOrderTarget& Target, FTCFOrderResult& OutResult);
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Orders")
	bool SubmitSelectedSquadsOrder(FGameplayTag OrderTag, const FTCFOrderTarget& Target, TArray<FTCFOrderResult>& OutResults);

	UPROPERTY(BlueprintAssignable, Category = "TCF|Orders")
	FOnTCFPlayerOrderSubmitted OnOrderSubmitted;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> PlayerSelectionComponent;

	UPROPERTY(VisibleInstanceOnly, Category = "TCF|Orders")
	int32 NextOrderSequence = 1;
	
	bool SubmitSquadOrder(
	UTCFOrderSubsystem& OrderSubsystem,
	ATCFSquadActor* SourceSquad,
	FGameplayTag OrderTag,
	const FTCFOrderTarget& Target,
	FTCFOrderResult& OutResult);

	UTCFPlayerSelectionComponent* FindSelectionComponent() const;
};
