//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/TCFSquadAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UTCFSquadAttributeSet::UTCFSquadAttributeSet()
{
	InitMorale(100.0f);
	InitSuppression(0.0f);
	InitCohesion(100.0f);
	InitStamina(100.0f);
	InitAccuracy(1.0f);
	InitDefense(1.0f);
	InitMovementSpeed(400.0f);
	InitCapturePower(1.0f);
}

void UTCFSquadAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UTCFSquadAttributeSet, Morale, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCFSquadAttributeSet, Suppression, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCFSquadAttributeSet, Cohesion, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCFSquadAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCFSquadAttributeSet, Accuracy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCFSquadAttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCFSquadAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTCFSquadAttributeSet, CapturePower, COND_None, REPNOTIFY_Always);
}

void UTCFSquadAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMoraleAttribute()
		|| Attribute == GetSuppressionAttribute()
		|| Attribute == GetCohesionAttribute()
		|| Attribute == GetStaminaAttribute())
	{
		ClampResourceAttribute(NewValue);
		return;
	}

	if (Attribute == GetAccuracyAttribute()
		|| Attribute == GetDefenseAttribute()
		|| Attribute == GetMovementSpeedAttribute()
		|| Attribute == GetCapturePowerAttribute())
	{
		ClampNonNegativeAttribute(NewValue);
	}
}

void UTCFSquadAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float ClampedValue = 0.0f;

	if (Data.EvaluatedData.Attribute == GetMoraleAttribute())
	{
		ClampedValue = GetMorale();
		ClampResourceAttribute(ClampedValue);
		SetMorale(ClampedValue);
		return;
	}

	if (Data.EvaluatedData.Attribute == GetSuppressionAttribute())
	{
		ClampedValue = GetSuppression();
		ClampResourceAttribute(ClampedValue);
		SetSuppression(ClampedValue);
		return;
	}

	if (Data.EvaluatedData.Attribute == GetCohesionAttribute())
	{
		ClampedValue = GetCohesion();
		ClampResourceAttribute(ClampedValue);
		SetCohesion(ClampedValue);
		return;
	}

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		ClampedValue = GetStamina();
		ClampResourceAttribute(ClampedValue);
		SetStamina(ClampedValue);
		return;
	}

	if (Data.EvaluatedData.Attribute == GetAccuracyAttribute())
	{
		ClampedValue = GetAccuracy();
		ClampNonNegativeAttribute(ClampedValue);
		SetAccuracy(ClampedValue);
		return;
	}

	if (Data.EvaluatedData.Attribute == GetDefenseAttribute())
	{
		ClampedValue = GetDefense();
		ClampNonNegativeAttribute(ClampedValue);
		SetDefense(ClampedValue);
		return;
	}

	if (Data.EvaluatedData.Attribute == GetMovementSpeedAttribute())
	{
		ClampedValue = GetMovementSpeed();
		ClampNonNegativeAttribute(ClampedValue);
		SetMovementSpeed(ClampedValue);
		return;
	}

	if (Data.EvaluatedData.Attribute == GetCapturePowerAttribute())
	{
		ClampedValue = GetCapturePower();
		ClampNonNegativeAttribute(ClampedValue);
		SetCapturePower(ClampedValue);
	}
}

void UTCFSquadAttributeSet::OnRep_Morale(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFSquadAttributeSet, Morale, OldValue);
}

void UTCFSquadAttributeSet::OnRep_Suppression(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFSquadAttributeSet, Suppression, OldValue);
}

void UTCFSquadAttributeSet::OnRep_Cohesion(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFSquadAttributeSet, Cohesion, OldValue);
}

void UTCFSquadAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFSquadAttributeSet, Stamina, OldValue);
}

void UTCFSquadAttributeSet::OnRep_Accuracy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFSquadAttributeSet, Accuracy, OldValue);
}

void UTCFSquadAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFSquadAttributeSet, Defense, OldValue);
}

void UTCFSquadAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFSquadAttributeSet, MovementSpeed, OldValue);
}

void UTCFSquadAttributeSet::OnRep_CapturePower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTCFSquadAttributeSet, CapturePower, OldValue);
}

void UTCFSquadAttributeSet::ClampResourceAttribute(float& NewValue)
{
	NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
}

void UTCFSquadAttributeSet::ClampNonNegativeAttribute(float& NewValue)
{
	NewValue = FMath::Max(0.0f, NewValue);
}
