//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/TCFOrderTypes.h"
#include "TCFSquadDebugComponent.generated.h"

class ATCFSquadActor;
class UTCFPlayerOrderComponent;
class UTCFPlayerSelectionComponent;

UCLASS(ClassGroup = (TCFDebug), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFSquadDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFSquadDebugComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Debug")
	void LogSelectedSquadState() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Debug")
	void LogSquadState(const ATCFSquadActor* Squad) const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Debug")
	void LogOrderSubmission(const FTCFSquadOrderRequest& Request, const FTCFOrderResult& Result) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Debug")
	bool bLogSelectionChanges = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Debug")
	bool bLogOrderSubmissions = true;

private:
	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> ObservedSelectionComponent;

	UPROPERTY()
	TObjectPtr<UTCFPlayerOrderComponent> ObservedOrderComponent;

	UFUNCTION()
	void HandleSelectedSquadChanged(ATCFSquadActor* SelectedSquad);

	UFUNCTION()
	void HandleOrderSubmitted(FTCFSquadOrderRequest Request, FTCFOrderResult Result);

	UTCFPlayerSelectionComponent* FindSelectionComponent() const;
	UTCFPlayerOrderComponent* FindOrderComponent() const;

	static FString BuildOrderTargetDebugString(const FTCFOrderTarget& Target);
	static FString BuildOrderResultDebugString(const FTCFOrderResult& Result);
};