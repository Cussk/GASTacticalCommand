//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/TCFGameplayAbility_OrderBase.h"
#include "TCFGameplayAbility_BasicAttack.generated.h"

class ATCFSquadActor;
class UGameplayEffect;

UCLASS()
class GASTACTICALCOMMAND_API UTCFGameplayAbility_BasicAttack : public UTCFGameplayAbility_OrderBase
{
	GENERATED_BODY()

public:
	UTCFGameplayAbility_BasicAttack();

protected:
	virtual void HandleOrderActivated() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Basic Attack")
	TArray<TSubclassOf<UGameplayEffect>> TargetAttackEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Basic Attack", meta = (ClampMin = "0.0"))
	float AttackEffectLevel = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Basic Attack", meta = (ClampMin = "0.0"))
	float FallbackAttackRange = 1200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Basic Attack")
	bool bRequireEnemyTarget = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Basic Attack")
	bool bRequireLineOfSight = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Basic Attack")
	TEnumAsByte<ECollisionChannel> LineOfSightTraceChannel = ECC_Visibility;

private:
	ATCFSquadActor* GetTargetSquad() const;

	float GetAttackRange() const;
	bool IsTargetInRange(const ATCFSquadActor& SourceSquad, const ATCFSquadActor& TargetSquad) const;
	bool ShouldRequireLineOfSight() const;
	bool HasLineOfSightToTarget(const ATCFSquadActor& SourceSquad, const ATCFSquadActor& TargetSquad) const;
	bool IsValidEnemyTarget(const ATCFSquadActor& SourceSquad, const ATCFSquadActor& TargetSquad) const;
};