//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Types/TCFOrderTypes.h"
#include "Abilities/GameplayAbility.h"
#include "TCFOrderDefinition.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UTexture2D;

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFOrderDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Identity")
	FName OrderId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Identity", meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Tags")
	FGameplayTag OrderTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Tags")
	FGameplayTagContainer RequiredSourceTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Tags")
	FGameplayTagContainer BlockedSourceTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Tags")
	FGameplayTagContainer GrantedStateTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Ability")
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Cost")
	FTCFOrderCostConfig Cost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Cooldown", meta = (ClampMin = "0.0"))
	float CooldownSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Duration", meta = (ClampMin = "0.0"))
	float DurationSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Targeting")
	FTCFOrderTargetingConfig Targeting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Effects")
	TArray<TSubclassOf<UGameplayEffect>> SourceEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Effects")
	TArray<TSubclassOf<UGameplayEffect>> TargetEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Cue")
	FGameplayTag GameplayCueTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Presentation")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Debug")
	bool bShowInDebugOrderList = true;

	bool IsValidDefinition() const
	{
		return !OrderId.IsNone() && OrderTag.IsValid();
	}

	bool HasAbilityClass() const
	{
		return AbilityClass != nullptr;
	}
};