//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Styling/SlateBrush.h"
#include "Types/TCFEconomyTypes.h"
#include "TCFUIViewTypes.generated.h"

class UTCFConstructionOptionDefinition;
class UTCFProductionOptionDefinition;

UENUM(BlueprintType)
enum class ETCFProductionOptionAvailability : uint8
{
	Available,
	QueueFull,
	InsufficientResources,
	Unavailable
};

UENUM(BlueprintType)
enum class ETCFActionAvailability : uint8
{
	Available,
	Unavailable,
	Locked,
	InsufficientResources,
	Blocked
};

USTRUCT(BlueprintType)
struct FTCFActionButtonViewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|UI")
	FName ActionId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|UI")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|UI")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|UI")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|UI")
	ETCFActionAvailability Availability = ETCFActionAvailability::Unavailable;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|UI")
	FText DisabledReason;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|UI")
	bool bCanExecute = false;
};

USTRUCT(BlueprintType)
struct FTCFBuildOptionViewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	TObjectPtr<UTCFConstructionOptionDefinition> ConstructionOption = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	TArray<FTCFResourceAmount> Cost;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	float RequiredConstructionWork = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	FGameplayTagContainer RequiredTags;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	FGameplayTagContainer BlockedTags;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	ETCFActionAvailability Availability = ETCFActionAvailability::Unavailable;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	FText DisabledReason;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Build")
	bool bCanRequest = false;
};

USTRUCT(BlueprintType)
struct FTCFResourceTooltipViewData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|UI")
	FGameplayTag ResourceTag;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|UI")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|UI")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|UI")
	FSlateBrush IconBrush;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Resources|UI")
	int32 CurrentAmount = 0;
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