//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/TCFPlacementGridTypes.h"
#include "TCFRTSPlacementGridSubsystem.generated.h"

UCLASS(Config = Game)
class GASTACTICALCOMMAND_API UTCFRTSPlacementGridSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TCF|Placement Grid")
	float GetCellSize() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Placement Grid")
	FVector GetGridOrigin() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Placement Grid")
	FIntPoint WorldToGridCell(FVector WorldLocation) const;

	UFUNCTION(BlueprintPure, Category = "TCF|Placement Grid")
	FVector GridCellToWorldCenter(FIntPoint GridCell) const;

	UFUNCTION(BlueprintPure, Category = "TCF|Placement Grid")
	FIntPoint GetAnchorCellForWorldLocation(FVector WorldLocation, FIntPoint FootprintSize) const;

	UFUNCTION(BlueprintPure, Category = "TCF|Placement Grid")
	FVector GetFootprintCenterWorldLocation(FIntPoint AnchorCell, FIntPoint FootprintSize) const;

	UFUNCTION(BlueprintPure, Category = "TCF|Placement Grid")
	FVector SnapWorldLocationToFootprintCenter(FVector WorldLocation, FIntPoint FootprintSize) const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Placement Grid")
	void GetCellsForFootprint(FIntPoint AnchorCell, FIntPoint FootprintSize, TArray<FIntPoint>& OutCells) const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Placement Grid")
	bool ValidateFootprint(
		FIntPoint AnchorCell,
		FIntPoint FootprintSize,
		const AActor* IgnoredActor,
		FTCFPlacementGridValidationResult& OutResult);

	UFUNCTION(BlueprintCallable, Category = "TCF|Placement Grid")
	bool ValidateFootprintAtWorldLocation(
		FVector WorldLocation,
		FIntPoint FootprintSize,
		const AActor* IgnoredActor,
		FIntPoint& OutAnchorCell,
		FTCFPlacementGridValidationResult& OutResult);

	UFUNCTION(BlueprintCallable, Category = "TCF|Placement Grid")
	bool ReserveFootprint(
		AActor* OwnerActor,
		FIntPoint AnchorCell,
		FIntPoint FootprintSize,
		bool bReplaceExistingReservation,
		FTCFPlacementGridValidationResult& OutResult);

	UFUNCTION(BlueprintCallable, Category = "TCF|Placement Grid")
	bool ReserveFootprintAtWorldLocation(
		AActor* OwnerActor,
		FVector WorldLocation,
		FIntPoint FootprintSize,
		bool bReplaceExistingReservation,
		FIntPoint& OutAnchorCell,
		FTCFPlacementGridValidationResult& OutResult);

	UFUNCTION(BlueprintCallable, Category = "TCF|Placement Grid")
	void ReleaseFootprintForActor(AActor* OwnerActor);

	UFUNCTION(BlueprintCallable, Category = "TCF|Placement Grid")
	bool IsCellOccupied(FIntPoint GridCell);

	UFUNCTION(BlueprintCallable, Category = "TCF|Placement Grid")
	AActor* GetCellOccupant(FIntPoint GridCell);

	UFUNCTION(BlueprintCallable, Category = "TCF|Placement Grid")
	bool GetReservedFootprintForActor(AActor* OwnerActor, FTCFPlacementGridReservation& OutReservation) const;

protected:
	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadOnly, Category = "TCF|Placement Grid", meta = (ClampMin = "1.0"))
	float CellSize = 300.0f;

	UPROPERTY(EditDefaultsOnly, Config, BlueprintReadOnly, Category = "TCF|Placement Grid")
	FVector GridOrigin = FVector::ZeroVector;

private:
	TMap<FIntPoint, TWeakObjectPtr<AActor>> OccupiedCells;
	TMap<TWeakObjectPtr<AActor>, FTCFPlacementGridReservation> ActorReservations;

	FIntPoint GetSafeFootprintSize(FIntPoint FootprintSize) const;
	bool HasValidCellSize() const;

	void CompactInvalidReservations();
};