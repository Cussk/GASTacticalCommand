//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFEconomyTypes.h"
#include "TCFProductionUIViewTypes.generated.h"

class UTCFProductionOptionDefinition;

UENUM(BlueprintType)
enum class ETCFProductionOptionAvailability : uint8
{
	Available,
	QueueFull,
	InsufficientResources,
	Unavailable
};

USTRUCT(BlueprintType)
struct FTCFProductionOptionViewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	TObjectPtr<UTCFProductionOptionDefinition> ProductionOption = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	TArray<FTCFResourceAmount> Cost;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	float ProductionTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	bool bCanRequest = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	bool bCanAfford = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	bool bHasQueueSpace = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	FText DisabledReason;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	ETCFProductionOptionAvailability Availability = ETCFProductionOptionAvailability::Unavailable;
};

USTRUCT(BlueprintType)
struct FTCFProductionQueueItemViewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	TObjectPtr<UTCFProductionOptionDefinition> ProductionOption = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	float CompletedProductionWork = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	float RequiredProductionWork = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	float ProgressAlpha = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	bool bIsActiveItem = false;
};