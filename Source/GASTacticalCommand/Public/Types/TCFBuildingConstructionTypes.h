// Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/TCFPlacementGridTypes.h"
#include "TCFBuildingConstructionTypes.generated.h"

class UTCFBuildingDefinition;
class UTCFConstructionOptionDefinition;

USTRUCT(BlueprintType)
struct FTCFConstructionAccessResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	bool bCanAccess = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	bool bHasOption = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	bool bHasBuildingDefinition = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	bool bMeetsRequiredTags = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	bool bBlockedByTags = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	FGameplayTagContainer MissingRequiredTags;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	FGameplayTagContainer MatchingBlockedTags;

	static FTCFConstructionAccessResult Success()
	{
		FTCFConstructionAccessResult Result;
		Result.bCanAccess = true;
		Result.bHasOption = true;
		Result.bHasBuildingDefinition = true;
		Result.bMeetsRequiredTags = true;
		Result.bBlockedByTags = false;
		return Result;
	}
};

USTRUCT(BlueprintType)
struct FTCFBuildingConstructionRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	TObjectPtr<UTCFBuildingDefinition> BuildingDefinition = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	FVector PlacementLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	FIntPoint AnchorCell = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	FTCFPlacementGridValidationResult PlacementValidationResult;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Construction")
	TObjectPtr<UTCFConstructionOptionDefinition> ConstructionOption = nullptr;

	bool IsValid() const
	{
		return ConstructionOption != nullptr
			&& BuildingDefinition != nullptr
			&& PlacementValidationResult.bIsValid;
	}
};