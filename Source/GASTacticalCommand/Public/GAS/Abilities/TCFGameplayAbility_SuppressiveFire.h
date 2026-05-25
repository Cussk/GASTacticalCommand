//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/TCFGameplayAbility_OrderBase.h"
#include "TCFGameplayAbility_SuppressiveFire.generated.h"

class ATCFSquadActor;
class UGameplayEffect;

UCLASS()
class GASTACTICALCOMMAND_API UTCFGameplayAbility_SuppressiveFire : public UTCFGameplayAbility_OrderBase
{
	GENERATED_BODY()

public:
	UTCFGameplayAbility_SuppressiveFire();

protected:
	virtual void HandleOrderActivated() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Suppressive Fire")
	TArray<TSubclassOf<UGameplayEffect>> TargetSuppressionEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Suppressive Fire")
	TArray<TSubclassOf<UGameplayEffect>> SourceFireEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Suppressive Fire", meta = (ClampMin = "0.0"))
	float SuppressiveFireEffectLevel = 1.0f;

private:
	bool TryGetSuppressionArea(FVector& OutLocation, float& OutRadius) const;
	void ApplySourceFireEffects() const;
	void ApplyTargetSuppressionEffects(const TArray<ATCFSquadActor*>& Targets) const;
};
