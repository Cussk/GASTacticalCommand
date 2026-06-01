//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "TCFPlacementGridTypes.generated.h"

UENUM(BlueprintType)
enum class ETCFPlacementGridValidationFailure : uint8
{
	None,
	InvalidCellSize,
	InvalidFootprint,
	CellOccupied
};

USTRUCT(BlueprintType)
struct FTCFPlacementGridValidationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Placement Grid")
	bool bIsValid = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Placement Grid")
	ETCFPlacementGridValidationFailure FailureReason = ETCFPlacementGridValidationFailure::None;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Placement Grid")
	FIntPoint FailedCell = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Placement Grid")
	TObjectPtr<AActor> BlockingActor = nullptr;

	static FTCFPlacementGridValidationResult Success()
	{
		FTCFPlacementGridValidationResult Result;
		Result.bIsValid = true;
		return Result;
	}

	static FTCFPlacementGridValidationResult Failure(
		ETCFPlacementGridValidationFailure Reason,
		FIntPoint InFailedCell = FIntPoint::ZeroValue,
		AActor* InBlockingActor = nullptr)
	{
		FTCFPlacementGridValidationResult Result;
		Result.bIsValid = false;
		Result.FailureReason = Reason;
		Result.FailedCell = InFailedCell;
		Result.BlockingActor = InBlockingActor;
		return Result;
	}
};

USTRUCT(BlueprintType)
struct FTCFPlacementGridReservation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Placement Grid")
	TObjectPtr<AActor> OwnerActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Placement Grid")
	FIntPoint AnchorCell = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Placement Grid")
	FIntPoint FootprintSize = FIntPoint(1, 1);

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Placement Grid")
	TArray<FIntPoint> OccupiedCells;
};