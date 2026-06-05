//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/TCFOrderTypes.h"
#include "TCFDebugTypes.generated.h"

USTRUCT(BlueprintType)
struct FTCFDebugEconomySnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bHasResourceBank = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 Materials = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 Energy = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 ResearchData = 0;
};

USTRUCT(BlueprintType)
struct FTCFDebugWorkerSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bHasSelectedSquad = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bIsWorker = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	TArray<FString> WorkerLines;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	TArray<FString> CommandLines;
};


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
struct FTCFDebugBuildingSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bHasBuilding = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString ActorName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString RuntimeState;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FGameplayTag BuildingTypeTag;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FGameplayTagContainer BuildingRoleTags;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bASCValid = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float Health = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float MaxHealth = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float Defense = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float ConstructionWorkCompleted = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float RequiredConstructionWork = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float ConstructionProgressAlpha = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bCanReceiveConstructionWork = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bHasReservedPlacementFootprint = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FIntPoint ReservedPlacementAnchorCell = FIntPoint::ZeroValue;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bHasAffiliation = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 OwnerId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 TeamId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FGameplayTag FactionTag;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString RelationshipToSelectedSquad;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString OwnedTags;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	TArray<FString> ActiveEffectLines;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString Source;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bHasProductionComponent = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 ProductionQueueCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	int32 MaxProductionQueueSize = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString ActiveProductionName;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float ActiveProductionCompletedWork = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float ActiveProductionRequiredWork = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	float ActiveProductionProgressAlpha = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	TArray<FString> ProductionQueueLines;
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
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FTCFDebugEconomySnapshot Economy;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FTCFDebugWorkerSnapshot Worker;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FTCFDebugBuildingSnapshot HoveredBuilding;
	
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	FString SquadSource;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bInspectionOnly = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Debug")
	bool bCommandable = false;
};