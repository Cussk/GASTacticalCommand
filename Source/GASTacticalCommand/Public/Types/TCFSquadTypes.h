//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once
#include "GameplayTagContainer.h"
#include "TCFSquadTypes.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct FTCFSquadAttributeDefaults
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attributes")
	float Morale = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attributes")
	float Suppression = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attributes")
	float Cohesion = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attributes")
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attributes")
	float Accuracy = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attributes")
	float Defense = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attributes")
	float MovementSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attributes")
	float CapturePower = 1.0f;
};

USTRUCT(BlueprintType)
struct FTCFSquadAbilityGrant
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Abilities")
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Abilities", meta = (ClampMin = "1"))
	int32 AbilityLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Abilities")
	FGameplayTag InputTag;
};