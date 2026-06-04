//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCFRTSSelectionBoxComponent.generated.h"

class UTCFRTSHoverContextComponent;
class UTCFRTSSelectionBoxWidget;
class ATCFSquadActor;
class APlayerController;
class UTCFPlayerSelectionComponent;

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFRTSSelectionBoxComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFRTSSelectionBoxComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Selection")
	void BeginSelection();

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Selection")
	void EndSelection(bool bAppendSelection);

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Selection")
	void CancelSelection();

	UFUNCTION(BlueprintPure, Category = "TCF|RTS Selection")
	bool IsDraggingSelection() const;

	UFUNCTION(BlueprintPure, Category = "TCF|RTS Selection")
	void GetSelectionRectangle(FVector2D& OutStart, FVector2D& OutEnd) const;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|UI")
	TSubclassOf<UTCFRTSSelectionBoxWidget> SelectionBoxWidgetClass;

	UPROPERTY()
	TObjectPtr<UTCFRTSSelectionBoxWidget> SelectionBoxWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Selection", meta = (ClampMin = "0.0"))
	float ClickDragThresholdPixels = 8.0f;

private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> SelectionComponent;
	
	UPROPERTY()
	TObjectPtr<UTCFRTSHoverContextComponent> HoverContextComponent;

	FVector2D DragStartScreenPosition = FVector2D::ZeroVector;
	FVector2D DragEndScreenPosition = FVector2D::ZeroVector;

	bool bSelectionActive = false;

	bool TryGetMouseScreenPosition(FVector2D& OutScreenPosition) const;

	ATCFSquadActor* GetSquadUnderCursor() const;
	void GetSquadsInsideSelectionBox(TArray<ATCFSquadActor*>& OutSquads) const;
	bool IsSquadInsideSelectionBox(const ATCFSquadActor& Squad) const;
	bool TryInspectHoveredBuilding() const;

	bool GetSelectionScreenBounds(FVector2D& OutMinScreenPosition, FVector2D& OutMaxScreenPosition) const;
	bool GetActorScreenBounds(const AActor& Actor, FVector2D& OutMinScreenPosition, FVector2D& OutMaxScreenPosition) const;
	static bool DoScreenRectanglesOverlap(const FVector2D& AMin, const FVector2D& AMax, const FVector2D& BMin, const FVector2D& BMax);
	
	void CreateSelectionBoxWidget();
};