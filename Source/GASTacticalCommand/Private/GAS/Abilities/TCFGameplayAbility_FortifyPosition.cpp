//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_FortifyPosition.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

UTCFGameplayAbility_FortifyPosition::UTCFGameplayAbility_FortifyPosition()
{
	bCommitAbilityOnOrderActivated = true;
	bEndAbilityAfterOrderActivated = true;
}

void UTCFGameplayAbility_FortifyPosition::HandleOrderActivated()
{
	ApplyFortifyEffectsToSelf();

	Super::HandleOrderActivated();
}

void UTCFGameplayAbility_FortifyPosition::ApplyFortifyEffectsToSelf() const
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (!AbilitySystem)
	{
		return;
	}

	for (const TSubclassOf<UGameplayEffect>& EffectClass : SelfFortifyEffects)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectContextHandle ContextHandle = AbilitySystem->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		const FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(
			EffectClass,
			FortifyEffectLevel,
			ContextHandle);

		if (SpecHandle.IsValid())
		{
			AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}