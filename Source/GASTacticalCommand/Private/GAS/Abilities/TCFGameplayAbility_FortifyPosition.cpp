//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_FortifyPosition.h"

UTCFGameplayAbility_FortifyPosition::UTCFGameplayAbility_FortifyPosition()
{
	bCommitAbilityOnOrderActivated = true;
	bEndAbilityAfterOrderActivated = true;
}

void UTCFGameplayAbility_FortifyPosition::HandleOrderActivated()
{
	ApplyGameplayEffectsToSelf(SelfFortifyEffects, FortifyEffectLevel, this);

	Super::HandleOrderActivated();
}
