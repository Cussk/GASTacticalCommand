//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/TCFOrderTypes.h"
#include "TCFDebugTypes.generated.h"

USTRUCT(BlueprintType)
struct FTCFDebugOrderSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bHasOrder = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 OrderSequence = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FGameplayTag OrderTag;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString SourceName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString TargetSummary;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bSuccess = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FGameplayTag ResultTag;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FGameplayTagContainer BlockingTags;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FText FailureReason;
};

USTRUCT(BlueprintType)
struct FTCFDebugCapturePointSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bHasCapturePoint = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString ActorName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString State;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString OwnerSide;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString PendingSide;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float CaptureProgress = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float CaptureThreshold = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float CaptureRadius = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float DistanceFromSelectedSquad = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 OccupyingSquadCount = 0;
};

USTRUCT(BlueprintType)
struct FTCFDebugSquadSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bHasSelectedSquad = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString ActorName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bInitialized = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FGameplayTag RoleTag;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bHasAffiliation = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 OwnerId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 TeamId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FGameplayTag FactionTag;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	TArray<FString> RelationshipLines;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FTCFDebugCapturePointSnapshot NearestCapturePoint;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString OwnedTags;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	TArray<FString> AttributeLines;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	TArray<FString> AbilityLines;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	TArray<FString> ActiveEffectLines;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FTCFDebugOrderSnapshot LastOrder;
};