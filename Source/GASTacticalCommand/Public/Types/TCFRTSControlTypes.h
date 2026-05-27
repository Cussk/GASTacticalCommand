//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFAffiliationTypes.h"
#include "TCFRTSControlTypes.generated.h"

UENUM(BlueprintType)
enum class ETCFRTSHoverTargetType : uint8
{
	None,
	Ground,
	Squad,
	CapturePoint,
	ResourceNode,
	ProductionBuilding,
	OtherActor
};

UENUM(BlueprintType)
enum class ETCFRTSCursorState : uint8
{
	Default,
	MoveGround,
	SelectableOwn,
	SelectableFriendly,
	AttackEnemy,
	CapturePoint,
	ResourceNode,
	ProductionBuilding,
	InvalidTarget
};

UENUM(BlueprintType)
enum class ETCFRTSCommandIntentType : uint8
{
	None,
	Move,
	AttackTarget,
	CapturePointMove,
	GatherResource,
	ProductionBuildingInteraction,
	Invalid
};

USTRUCT(BlueprintType)
struct FTCFRTSHoverContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Hover")
	bool bHasHit = false;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Hover")
	ETCFRTSHoverTargetType TargetType = ETCFRTSHoverTargetType::None;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Hover")
	ETCFRTSCursorState CursorState = ETCFRTSCursorState::Default;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Hover")
	TObjectPtr<AActor> HoveredActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Hover")
	FVector WorldLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Hover")
	FVector WorldNormal = FVector::UpVector;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Hover")
	ETCFSquadRelationship RelationshipToPrimarySelection = ETCFSquadRelationship::Neutral;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Hover")
	bool bHasPrimarySelection = false;
};

USTRUCT(BlueprintType)
struct FTCFRTSCommandIntent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Command")
	ETCFRTSCommandIntentType IntentType = ETCFRTSCommandIntentType::None;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Command")
	ETCFRTSHoverTargetType TargetType = ETCFRTSHoverTargetType::None;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Command")
	ETCFRTSCursorState CursorState = ETCFRTSCursorState::Default;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Command")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Command")
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Command")
	ETCFSquadRelationship RelationshipToPrimarySelection = ETCFSquadRelationship::Neutral;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Command")
	int32 SelectedSquadCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|RTS Command")
	bool bWasExecuted = false;
};

USTRUCT(BlueprintType)
struct FTCFRTSCursorVisual
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	ETCFRTSCursorState CursorState = ETCFRTSCursorState::Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TObjectPtr<UTexture2D> CursorTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	FVector2D DrawSize = FVector2D(64.0f, 64.0f);

	// Pixel offset from the top-left of the drawn image to the actual click point.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	FVector2D HotSpot = FVector2D(16.0f, 16.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	FLinearColor Tint = FLinearColor::White;
};
