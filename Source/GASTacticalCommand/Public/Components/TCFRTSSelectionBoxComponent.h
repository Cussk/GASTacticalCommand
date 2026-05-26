//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCFRTSSelectionBoxComponent.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Selection", meta = (ClampMin = "1.0"))
	float DragThresholdPixels = 8.0f;

private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> SelectionComponent;

	FVector2D DragStartScreenPosition = FVector2D::ZeroVector;
	FVector2D DragEndScreenPosition = FVector2D::ZeroVector;

	bool bSelectionActive = false;

	bool TryGetMouseScreenPosition(FVector2D& OutScreenPosition) const;
	bool WasDragSelection() const;

	ATCFSquadActor* GetSquadUnderCursor() const;
	void GetSquadsInsideSelectionBox(TArray<ATCFSquadActor*>& OutSquads) const;
	bool IsSquadInsideSelectionBox(const ATCFSquadActor& Squad) const;
};