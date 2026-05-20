//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/TCFGameplayAbility_OrderBase.h"
#include "TCFGameplayAbility_FortifyPosition.generated.h"

class UGameplayEffect;

UCLASS()
class GASTACTICALCOMMAND_API UTCFGameplayAbility_FortifyPosition : public UTCFGameplayAbility_OrderBase
{
	GENERATED_BODY()

public:
	UTCFGameplayAbility_FortifyPosition();

protected:
	virtual void HandleOrderActivated() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Fortify")
	TArray<TSubclassOf<UGameplayEffect>> SelfFortifyEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Fortify", meta = (ClampMin = "0.0"))
	float FortifyEffectLevel = 1.0f;

private:
	void ApplyFortifyEffectsToSelf() const;
};