// Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Types/TCFEconomyTypes.h"
#include "TCFConstructionOptionDefinition.generated.h"

class UTexture2D;
class UTCFBuildingDefinition;

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFConstructionOptionDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintPure, Category = "TCF|Construction")
	UTCFBuildingDefinition* GetBuildingDefinition() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Construction")
	const TArray<FTCFResourceAmount>& GetEffectiveCost() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Construction")
	FText GetSafeDisplayName() const;
	
	UFUNCTION(BlueprintPure, Category = "TCF|Construction")
	FText GetDescription() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Construction")
	float GetRequiredConstructionWork() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction")
	TObjectPtr<UTCFBuildingDefinition> BuildingDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction|Economy")
	TArray<FTCFResourceAmount> ConstructionCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction|Requirements")
	FGameplayTagContainer RequiredCommanderTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction|Requirements")
	FGameplayTagContainer BlockedCommanderTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction|UI")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction|UI", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction|UI")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction|UI")
	FGameplayTag BuildMenuCategoryTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction|UI")
	bool bShowWhenLocked = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction|UI")
	int32 SortPriority = 0;
};