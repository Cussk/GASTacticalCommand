//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "TCFProductionTypes.generated.h"

class ATCFPlayerState;
class UTCFProductionOptionDefinition;

USTRUCT(BlueprintType)
struct FTCFProductionRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	TObjectPtr<UTCFProductionOptionDefinition> ProductionOption = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	TObjectPtr<ATCFPlayerState> RequestingPlayerState = nullptr;

	bool IsValid() const
	{
		return ProductionOption && RequestingPlayerState;
	}
};

USTRUCT(BlueprintType)
struct FTCFProductionQueueItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	TObjectPtr<UTCFProductionOptionDefinition> ProductionOption = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	TObjectPtr<ATCFPlayerState> RequestingPlayerState = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	float CompletedProductionWork = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	float RequiredProductionWork = 0.0f;

	bool IsValid() const
	{
		return ProductionOption
			&& RequestingPlayerState
			&& RequiredProductionWork >= 0.0f;
	}

	bool IsComplete() const
	{
		return IsValid() && CompletedProductionWork >= RequiredProductionWork;
	}

	float GetProgressAlpha() const
	{
		if (RequiredProductionWork <= 0.0f)
		{
			return IsValid() ? 1.0f : 0.0f;
		}

		return FMath::Clamp(CompletedProductionWork / RequiredProductionWork, 0.0f, 1.0f);
	}
};