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