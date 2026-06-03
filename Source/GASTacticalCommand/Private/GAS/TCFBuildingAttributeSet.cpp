//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/TCFBuildingAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UTCFBuildingAttributeSet::UTCFBuildingAttributeSet()
{
	InitHealth(500.0f);
	InitMaxHealth(500.0f);
	InitDefense(1.0f);
}

void UTCFBuildingAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UTCFBuildingAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCFBuildingAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCFBuildingAttributeSet, Defense, COND_None, REPNOTIFY_Always);
}

void UTCFBuildingAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
		return;
	}

	if (Attribute == GetMaxHealthAttribute()
		|| Attribute == GetDefenseAttribute())
	{
		ClampNonNegativeAttribute(NewValue);
	}
}

void UTCFBuildingAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		return;
	}

	if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Max(0.0f, GetMaxHealth()));
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		return;
	}

	if (Data.EvaluatedData.Attribute == GetDefenseAttribute())
	{
		float ClampedValue = GetDefense();
		ClampNonNegativeAttribute(ClampedValue);
		SetDefense(ClampedValue);
	}
}

void UTCFBuildingAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFBuildingAttributeSet, Health, OldValue);
}

void UTCFBuildingAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFBuildingAttributeSet, MaxHealth, OldValue);
}

void UTCFBuildingAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFBuildingAttributeSet, Defense, OldValue);
}

void UTCFBuildingAttributeSet::ClampNonNegativeAttribute(float& NewValue)
{
	NewValue = FMath::Max(0.0f, NewValue);
}