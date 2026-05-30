// Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TCFPlayerState.generated.h"

class UTCFPlayerResourceBankComponent;

/**
 * 
 */
UCLASS()
class GASTACTICALCOMMAND_API ATCFPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ATCFPlayerState();
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Economy")
	UTCFPlayerResourceBankComponent* GetPlayerResourceBankComponent() const;	
	
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFPlayerResourceBankComponent> PlayerResourceBankComponent;
};
