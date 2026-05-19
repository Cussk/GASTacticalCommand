//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCFSquadDebugComponent.generated.h"

class ATCFSquadActor;
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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Debug")
	bool bLogSelectionChanges = true;

private:
	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> ObservedSelectionComponent;

	UFUNCTION()
	void HandleSelectedSquadChanged(ATCFSquadActor* SelectedSquad);

	UTCFPlayerSelectionComponent* FindSelectionComponent() const;
};