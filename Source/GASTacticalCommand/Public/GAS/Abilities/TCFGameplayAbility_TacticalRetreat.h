//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/TCFGameplayAbility_OrderBase.h"
#include "TCFGameplayAbility_TacticalRetreat.generated.h"

class UGameplayEffect;

UCLASS()
class GASTACTICALCOMMAND_API UTCFGameplayAbility_TacticalRetreat : public UTCFGameplayAbility_OrderBase
{
	GENERATED_BODY()

public:
	UTCFGameplayAbility_TacticalRetreat();

protected:
	virtual void HandleOrderActivated() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tactical Retreat")
	TArray<TSubclassOf<UGameplayEffect>> SelfRetreatEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Tactical Retreat", meta = (ClampMin = "0.0"))
	float RetreatEffectLevel = 1.0f;
};
