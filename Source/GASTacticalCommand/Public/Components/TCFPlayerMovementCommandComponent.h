//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCFPlayerMovementCommandComponent.generated.h"

class ATCFSquadActor;
class UTCFPlayerSelectionComponent;

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFPlayerMovementCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFPlayerMovementCommandComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Movement")
	bool MoveSelectedSquadToLocation(FVector WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "TCF|Movement")
	void StopSelectedSquadMovement();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> SelectionComponent;

	ATCFSquadActor* GetSelectedSquad() const;
};