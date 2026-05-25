//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_TacticalRetreat.h"

UTCFGameplayAbility_TacticalRetreat::UTCFGameplayAbility_TacticalRetreat()
{
	bCommitAbilityOnOrderActivated = true;
	bEndAbilityAfterOrderActivated = true;
}

void UTCFGameplayAbility_TacticalRetreat::HandleOrderActivated()
{
	ApplyGameplayEffectsToSelf(SelfRetreatEffects, RetreatEffectLevel, this);

	Super::HandleOrderActivated();
}
