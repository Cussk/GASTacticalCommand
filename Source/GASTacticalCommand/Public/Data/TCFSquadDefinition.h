//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Types/TCFSquadTypes.h"
#include "TCFSquadDefinition.generated.h"

class UGameplayEffect;
class UTexture2D;

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFSquadDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Identity")
	FName SquadId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Identity", meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Tags")
	FGameplayTag RoleTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Tags")
	FGameplayTagContainer StartupTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attributes")
	FTCFSquadAttributeDefaults AttributeDefaults;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Abilities")
	TArray<FTCFSquadAbilityGrant> StartupAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Effects")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Selection", meta = (ClampMin = "0.0"))
	float SelectionRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Presentation")
	TSoftObjectPtr<UTexture2D> Icon;

	bool IsValidDefinition() const
	{
		return !SquadId.IsNone() && RoleTag.IsValid();
	}
};
