//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TCFRTSCameraPawn.generated.h"

class UCameraComponent;
class USceneComponent;
class USpringArmComponent;

UCLASS()
class GASTACTICALCOMMAND_API ATCFRTSCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	ATCFRTSCameraPawn();

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Camera")
	void SetPanInput(FVector2D NewPanInput);

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Camera")
	void ClearPanInput();

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Camera")
	void AddZoomInput(float ZoomInput);

	UFUNCTION(BlueprintPure, Category = "TCF|RTS Camera")
	float GetCurrentZoomDistance() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (ClampMin = "0.0"))
	float PanSpeed = 2200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera")
	bool bScalePanSpeedByZoom = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (EditCondition = "bScalePanSpeedByZoom", ClampMin = "0.1"))
	float MinZoomPanSpeedMultiplier = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (EditCondition = "bScalePanSpeedByZoom", ClampMin = "0.1"))
	float MaxZoomPanSpeedMultiplier = 1.45f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (ClampMin = "100.0"))
	float MinZoomDistance = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (ClampMin = "100.0"))
	float MaxZoomDistance = 3500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (ClampMin = "1.0"))
	float ZoomStep = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (ClampMin = "1.0"))
	float ZoomInterpSpeed = 14.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera")
	bool bEnableEdgeScrolling = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (EditCondition = "bEnableEdgeScrolling", ClampMin = "1.0"))
	float EdgeScrollMarginPixels = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (EditCondition = "bEnableEdgeScrolling", ClampMin = "0.1"))
	float EdgeScrollSpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera")
	bool bClampCameraBounds = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (EditCondition = "bClampCameraBounds"))
	FVector2D MinCameraBounds = FVector2D(-10000.0f, -10000.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Camera", meta = (EditCondition = "bClampCameraBounds"))
	FVector2D MaxCameraBounds = FVector2D(10000.0f, 10000.0f);

private:
	FVector2D ManualPanInput = FVector2D::ZeroVector;
	FVector2D EdgePanInput = FVector2D::ZeroVector;

	float TargetZoomDistance = 1800.0f;

	void UpdateEdgeScrollInput();
	void UpdatePan(float DeltaSeconds);
	void UpdateZoom(float DeltaSeconds) const;
	void ApplyCameraBounds();

	float GetEffectivePanSpeed() const;
	float GetZoomAlpha() const;
};