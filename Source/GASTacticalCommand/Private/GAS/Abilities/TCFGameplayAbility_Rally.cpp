//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_Rally.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

UTCFGameplayAbility_Rally::UTCFGameplayAbility_Rally()
{
	bCommitAbilityOnOrderActivated = true;
	bEndAbilityAfterOrderActivated = true;
}

void UTCFGameplayAbility_Rally::HandleOrderActivated()
{
	ApplyRallyEffectsToSelf();

	Super::HandleOrderActivated();
}

void UTCFGameplayAbility_Rally::ApplyRallyEffectsToSelf() const
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystem)
	{
		return;
	}

	for (const TSubclassOf<UGameplayEffect>& EffectClass : SelfRallyEffects)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectContextHandle ContextHandle = AbilitySystem->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		const FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(
			EffectClass,
			RallyEffectLevel,
			ContextHandle);

		if (SpecHandle.IsValid())
		{
			AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}
