//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_Rally.h"

UTCFGameplayAbility_Rally::UTCFGameplayAbility_Rally()
{
	bCommitAbilityOnOrderActivated = true;
	bEndAbilityAfterOrderActivated = true;
}

void UTCFGameplayAbility_Rally::HandleOrderActivated()
{
	ApplyGameplayEffectsToSelf(SelfRallyEffects, RallyEffectLevel, this);

	Super::HandleOrderActivated();
}
