//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TCFEconomyTypes.generated.h"

USTRUCT(BlueprintType)
struct FTCFResourceAmount
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Economy")
	FGameplayTag ResourceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Economy", meta = (ClampMin = "0"))
	int32 Amount = 0;
};

USTRUCT(BlueprintType)
struct FTCFResourceTransactionResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Economy")
	bool bSuccess = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Economy")
	FGameplayTag FailedResourceType;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Economy")
	int32 CurrentAmount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Economy")
	int32 RequiredAmount = 0;

	static FTCFResourceTransactionResult Success()
	{
		FTCFResourceTransactionResult Result;
		Result.bSuccess = true;
		return Result;
	}

	static FTCFResourceTransactionResult Failure(
		FGameplayTag ResourceType,
		int32 InCurrentAmount,
		int32 InRequiredAmount)
	{
		FTCFResourceTransactionResult Result;
		Result.bSuccess = false;
		Result.FailedResourceType = ResourceType;
		Result.CurrentAmount = InCurrentAmount;
		Result.RequiredAmount = InRequiredAmount;
		return Result;
	}
};