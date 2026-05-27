//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/TCFRTSControlTypes.h"
#include "TCFRTSHoverContextComponent.generated.h"

class APlayerController;
class ATCFSquadActor;
class UTCFPlayerSelectionComponent;
class UTCFRelationshipSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFRTSHoverContextChanged, const FTCFRTSHoverContext&, NewHoverContext);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFRTSHoverContextComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFRTSHoverContextComponent();

	UFUNCTION(BlueprintPure, Category = "TCF|RTS Hover")
	const FTCFRTSHoverContext& GetCurrentHoverContext() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Hover")
	void ForceRefreshHoverContext();

	UPROPERTY(BlueprintAssignable, Category = "TCF|RTS Hover")
	FOnTCFRTSHoverContextChanged OnHoverContextChanged;

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Hover")
	TEnumAsByte<ECollisionChannel> HoverTraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Hover")
	bool bTraceComplex = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Hover")
	bool bApplyMouseCursor = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Hover")
	FName ResourceNodeActorTag = TEXT("RTS.ResourceNode");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Hover")
	FName ProductionBuildingActorTag = TEXT("RTS.ProductionBuilding");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TEnumAsByte<EMouseCursor::Type> DefaultCursor = EMouseCursor::Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TEnumAsByte<EMouseCursor::Type> MoveGroundCursor = EMouseCursor::Crosshairs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TEnumAsByte<EMouseCursor::Type> SelectableOwnCursor = EMouseCursor::Hand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TEnumAsByte<EMouseCursor::Type> SelectableFriendlyCursor = EMouseCursor::Hand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TEnumAsByte<EMouseCursor::Type> AttackEnemyCursor = EMouseCursor::Crosshairs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TEnumAsByte<EMouseCursor::Type> CapturePointCursor = EMouseCursor::GrabHand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TEnumAsByte<EMouseCursor::Type> ResourceNodeCursor = EMouseCursor::GrabHand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TEnumAsByte<EMouseCursor::Type> ProductionBuildingCursor = EMouseCursor::Hand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TEnumAsByte<EMouseCursor::Type> InvalidTargetCursor = EMouseCursor::SlashedCircle;

private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> SelectionComponent;

	UPROPERTY()
	TObjectPtr<UTCFRelationshipSubsystem> RelationshipSubsystem;

	FTCFRTSHoverContext CurrentHoverContext;

	void RefreshHoverContext();

	bool TraceHover(FHitResult& OutHitResult) const;
	FTCFRTSHoverContext BuildHoverContextFromHit(const FHitResult& HitResult) const;

	ETCFRTSHoverTargetType ResolveTargetType(const AActor* HitActor) const;
	ETCFRTSCursorState ResolveCursorState(const FTCFRTSHoverContext& HoverContext, const ATCFSquadActor* HoveredSquad) const;
	ETCFSquadRelationship ResolveRelationshipToPrimarySelection(const AActor* HoveredActor) const;

	void ApplyCursorState(ETCFRTSCursorState CursorState) const;
	EMouseCursor::Type GetMouseCursorForState(ETCFRTSCursorState CursorState) const;

	bool AreHoverContextsEquivalent(const FTCFRTSHoverContext& A, const FTCFRTSHoverContext& B) const;
};
