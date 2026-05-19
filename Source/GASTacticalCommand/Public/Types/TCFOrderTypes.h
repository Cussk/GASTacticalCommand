//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TCFOrderTypes.generated.h"

class AActor;

UENUM(BlueprintType)
enum class ETCFOrderTargetType : uint8
{
	None,
	Self,
	Actor,
	Location,
	Direction,
	Area
};

USTRUCT(BlueprintType)
struct FTCFOrderTarget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	ETCFOrderTargetType TargetType = ETCFOrderTargetType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	FVector TargetDirection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order", meta = (ClampMin = "0.0"))
	float Radius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order", meta = (ClampMin = "0.0", ClampMax = "360.0"))
	float ConeAngleDegrees = 0.0f;

	bool HasActorTarget() const
	{
		return IsValid(TargetActor);
	}

	bool HasLocationTarget() const
	{
		return TargetType == ETCFOrderTargetType::Location || TargetType == ETCFOrderTargetType::Area;
	}

	bool HasDirectionTarget() const
	{
		return TargetType == ETCFOrderTargetType::Direction && !TargetDirection.IsNearlyZero();
	}
};

USTRUCT(BlueprintType)
struct FTCFSquadOrderRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	FGameplayTag OrderTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	FTCFOrderTarget Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	FGameplayTagContainer ContextTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	int32 OrderSequence = 0;

	bool IsValidRequest() const
	{
		return OrderTag.IsValid() && IsValid(SourceActor);
	}
};

USTRUCT(BlueprintType)
struct FTCFOrderResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	bool bSuccess = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	FGameplayTag ResultTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	FGameplayTagContainer BlockingTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Order")
	FText FailureReason;

	static FTCFOrderResult Success(const FGameplayTag& InResultTag)
	{
		FTCFOrderResult Result;
		Result.bSuccess = true;
		Result.ResultTag = InResultTag;
		return Result;
	}

	static FTCFOrderResult Failure(const FGameplayTag& InResultTag, const FText& InReason)
	{
		FTCFOrderResult Result;
		Result.bSuccess = false;
		Result.ResultTag = InResultTag;
		Result.FailureReason = InReason;
		return Result;
	}

	static FTCFOrderResult FailureWithBlockingTags(
		const FGameplayTag& InResultTag,
		const FGameplayTagContainer& InBlockingTags,
		const FText& InReason)
	{
		FTCFOrderResult Result;
		Result.bSuccess = false;
		Result.ResultTag = InResultTag;
		Result.BlockingTags = InBlockingTags;
		Result.FailureReason = InReason;
		return Result;
	}
};

USTRUCT(BlueprintType)
struct FTCFOrderCostConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Cost", meta = (ClampMin = "0.0"))
	float StaminaCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Cost", meta = (ClampMin = "0.0"))
	float MoraleCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Cost", meta = (ClampMin = "0.0"))
	float CohesionCost = 0.0f;
};

USTRUCT(BlueprintType)
struct FTCFOrderTargetingConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Targeting")
	ETCFOrderTargetType TargetType = ETCFOrderTargetType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Targeting", meta = (ClampMin = "0.0"))
	float Range = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Targeting", meta = (ClampMin = "0.0"))
	float Radius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Targeting", meta = (ClampMin = "0.0", ClampMax = "360.0"))
	float ConeAngleDegrees = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Targeting")
	bool bRequiresLineOfSight = false;
};