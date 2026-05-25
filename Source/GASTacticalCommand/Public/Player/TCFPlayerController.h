//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TCFPlayerController.generated.h"

class UTCFPlayerMovementCommandComponent;
class UTCFPlayerOrderComponent;
class ATCFSquadActor;
class UTCFPlayerSelectionComponent;

UCLASS()
class GASTACTICALCOMMAND_API ATCFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATCFPlayerController();

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	UTCFPlayerSelectionComponent* GetPlayerSelectionComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Movement")
	UTCFPlayerMovementCommandComponent* GetPlayerMovementCommandComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Orders")
	UTCFPlayerOrderComponent* GetPlayerOrderComponent() const;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFPlayerSelectionComponent> PlayerSelectionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFPlayerMovementCommandComponent> PlayerMovementCommandComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFPlayerOrderComponent> PlayerOrderComponent;

private:
	void HandleSelectPressed();
	ATCFSquadActor* GetSquadUnderCursor() const;
};
