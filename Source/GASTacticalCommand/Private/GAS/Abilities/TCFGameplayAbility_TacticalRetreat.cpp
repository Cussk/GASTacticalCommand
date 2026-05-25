//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_TacticalRetreat.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

UTCFGameplayAbility_TacticalRetreat::UTCFGameplayAbility_TacticalRetreat()
{
	bCommitAbilityOnOrderActivated = true;
	bEndAbilityAfterOrderActivated = true;
}

void UTCFGameplayAbility_TacticalRetreat::HandleOrderActivated()
{
	ApplyRetreatEffectsToSelf();

	Super::HandleOrderActivated();
}

void UTCFGameplayAbility_TacticalRetreat::ApplyRetreatEffectsToSelf() const
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystem)
	{
		return;
	}

	for (const TSubclassOf<UGameplayEffect>& EffectClass : SelfRetreatEffects)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectContextHandle ContextHandle = AbilitySystem->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		const FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(
			EffectClass,
			RetreatEffectLevel,
			ContextHandle);

		if (SpecHandle.IsValid())
		{
			AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}
