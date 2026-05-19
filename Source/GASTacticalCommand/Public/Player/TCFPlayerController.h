//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TCFPlayerController.generated.h"

class UTCFSquadDebugComponent;
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

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFPlayerSelectionComponent> PlayerSelectionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFSquadDebugComponent> SquadDebugComponent;

private:
	void HandleSelectPressed();
	ATCFSquadActor* GetSquadUnderCursor() const;
};