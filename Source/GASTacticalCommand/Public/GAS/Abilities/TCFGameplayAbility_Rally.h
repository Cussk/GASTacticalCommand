//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "TCFGameplayAbility_OrderBase.h"
#include "TCFGameplayAbility_Rally.generated.h"

class UGameplayEffect;

UCLASS()
class GASTACTICALCOMMAND_API UTCFGameplayAbility_Rally : public UTCFGameplayAbility_OrderBase
{
	GENERATED_BODY()

public:
	UTCFGameplayAbility_Rally();

protected:
	virtual void HandleOrderActivated() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Rally")
	TArray<TSubclassOf<UGameplayEffect>> SelfRallyEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Rally", meta = (ClampMin = "0.0"))
	float RallyEffectLevel = 1.0f;

private:
	void ApplyRallyEffectsToSelf() const;
};
