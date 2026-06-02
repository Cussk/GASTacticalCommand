// Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "TCFPlayerState.generated.h"

class UAbilitySystemComponent;
class UTCFPlayerConstructionComponent;
class UTCFPlayerResourceBankComponent;

UCLASS()
class GASTACTICALCOMMAND_API ATCFPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ATCFPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Economy")
	UTCFPlayerResourceBankComponent* GetPlayerResourceBankComponent() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Construction")
	UTCFPlayerConstructionComponent* GetPlayerConstructionComponent() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UAbilitySystemComponent> CommanderAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFPlayerResourceBankComponent> PlayerResourceBankComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFPlayerConstructionComponent> PlayerConstructionComponent;
};