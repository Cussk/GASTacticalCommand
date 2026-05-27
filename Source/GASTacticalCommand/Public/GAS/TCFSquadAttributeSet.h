//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "TCFSquadAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GASTACTICALCOMMAND_API UTCFSquadAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UTCFSquadAttributeSet();
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UTCFSquadAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UTCFSquadAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Attributes", ReplicatedUsing = OnRep_Morale)
	FGameplayAttributeData Morale;
	ATTRIBUTE_ACCESSORS(UTCFSquadAttributeSet, Morale)

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Attributes", ReplicatedUsing = OnRep_Suppression)
	FGameplayAttributeData Suppression;
	ATTRIBUTE_ACCESSORS(UTCFSquadAttributeSet, Suppression)

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Attributes", ReplicatedUsing = OnRep_Cohesion)
	FGameplayAttributeData Cohesion;
	ATTRIBUTE_ACCESSORS(UTCFSquadAttributeSet, Cohesion)

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Attributes", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UTCFSquadAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Attributes", ReplicatedUsing = OnRep_Accuracy)
	FGameplayAttributeData Accuracy;
	ATTRIBUTE_ACCESSORS(UTCFSquadAttributeSet, Accuracy)

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Attributes", ReplicatedUsing = OnRep_Defense)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UTCFSquadAttributeSet, Defense)

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Attributes", ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(UTCFSquadAttributeSet, MovementSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Attributes", ReplicatedUsing = OnRep_CapturePower)
	FGameplayAttributeData CapturePower;
	ATTRIBUTE_ACCESSORS(UTCFSquadAttributeSet, CapturePower)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Morale(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Suppression(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Cohesion(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Accuracy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CapturePower(const FGameplayAttributeData& OldValue);

private:
	static void ClampResourceAttribute(float& NewValue);
	static void ClampNonNegativeAttribute(float& NewValue);
};
