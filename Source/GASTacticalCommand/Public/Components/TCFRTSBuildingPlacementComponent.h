//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/TCFPlacementGridTypes.h"
#include "TCFRTSBuildingPlacementComponent.generated.h"

class UTCFRTSSelectionBoxComponent;
class APlayerController;
class AStaticMeshActor;
class UMaterialInterface;
class UTCFBuildingDefinition;
class UTCFRTSHoverContextComponent;
class UTCFRTSPlacementGridSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnTCFBuildingPlacementStarted,
	UTCFBuildingDefinition*,
	BuildingDefinition);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTCFBuildingPlacementCanceled);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnTCFBuildingPlacementConfirmed,
	UTCFBuildingDefinition*,
	BuildingDefinition,
	FVector,
	PlacementLocation,
	FIntPoint,
	AnchorCell,
	FTCFPlacementGridValidationResult,
	ValidationResult);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFRTSBuildingPlacementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFRTSBuildingPlacementComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Building Placement")
	bool BeginBuildingPlacement(UTCFBuildingDefinition* BuildingDefinition);

	UFUNCTION(BlueprintCallable, Category = "TCF|Building Placement")
	void CancelBuildingPlacement();

	UFUNCTION(BlueprintCallable, Category = "TCF|Building Placement")
	bool ConfirmBuildingPlacement();

	UFUNCTION(BlueprintPure, Category = "TCF|Building Placement")
	bool IsPlacingBuilding() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building Placement")
	bool IsCurrentPlacementValid() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building Placement")
	UTCFBuildingDefinition* GetPendingBuildingDefinition() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building Placement")
	FVector GetCurrentPlacementLocation() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building Placement")
	FIntPoint GetCurrentAnchorCell() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building Placement")
	FTCFPlacementGridValidationResult GetCurrentValidationResult() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Building Placement")
	FOnTCFBuildingPlacementStarted OnBuildingPlacementStarted;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Building Placement")
	FOnTCFBuildingPlacementCanceled OnBuildingPlacementCanceled;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Building Placement")
	FOnTCFBuildingPlacementConfirmed OnBuildingPlacementConfirmed;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Building Placement")
	TEnumAsByte<ECollisionChannel> GroundTraceChannel = ECC_WorldStatic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Building Placement", meta = (ClampMin = "1000.0"))
	float GroundTraceDistance = 100000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Building Placement")
	float PreviewGroundOffset = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Building Placement")
	bool bShowInvalidPreview = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Building Placement")
	TObjectPtr<UMaterialInterface> DefaultValidPreviewMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Building Placement")
	TObjectPtr<UMaterialInterface> DefaultInvalidPreviewMaterial;

private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UTCFRTSHoverContextComponent> HoverContextComponent;
	
	UPROPERTY()
	TObjectPtr<UTCFRTSSelectionBoxComponent> SelectionBoxComponent;

	UPROPERTY()
	TObjectPtr<UTCFBuildingDefinition> PendingBuildingDefinition;

	UPROPERTY()
	TObjectPtr<AStaticMeshActor> PreviewActor;

	FTCFPlacementGridValidationResult CurrentValidationResult;
	FVector CurrentPlacementLocation = FVector::ZeroVector;
	FVector CurrentPlacementNormal = FVector::UpVector;
	FIntPoint CurrentAnchorCell = FIntPoint::ZeroValue;
	bool bCurrentPlacementValid = false;

	void RefreshPlacement();
	void RefreshCursorOverride() const;
	void RefreshPreview();
	void HidePreview() const;
	void DestroyPreview();

	bool TraceGround(FHitResult& OutHitResult) const;
	bool UpdatePlacementFromTrace();

	UStaticMesh* ResolvePreviewMesh() const;
	UMaterialInterface* ResolvePreviewMaterial() const;
	FVector ResolvePreviewScale() const;

	UTCFRTSPlacementGridSubsystem* GetPlacementGridSubsystem() const;
};