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
	bool MoveSelectedSquadsToLocation(FVector WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "TCF|Movement")
	void StopSelectedSquadMovement();

	UFUNCTION(BlueprintCallable, Category = "TCF|Movement")
	void StopSelectedSquadsMovement();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Movement", meta = (ClampMin = "0.0"))
	float MultiMoveSpacing = 180.0f;

private:
	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> SelectionComponent;

	ATCFSquadActor* GetSelectedSquad() const;
	void GetSelectedSquads(TArray<ATCFSquadActor*>& OutSelectedSquads) const;

	FVector GetFormationOffset(int32 Index, int32 Count) const;
};