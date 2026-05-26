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