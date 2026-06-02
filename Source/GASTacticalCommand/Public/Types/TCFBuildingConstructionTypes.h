// Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFPlacementGridTypes.h"
#include "TCFBuildingConstructionTypes.generated.h"

class UTCFBuildingDefinition;

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

	bool IsValid() const
	{
		return BuildingDefinition != nullptr && PlacementValidationResult.bIsValid;
	}
};