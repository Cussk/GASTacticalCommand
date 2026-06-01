//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Subsystems/TCFRTSPlacementGridSubsystem.h"

float UTCFRTSPlacementGridSubsystem::GetCellSize() const
{
	return CellSize;
}

FVector UTCFRTSPlacementGridSubsystem::GetGridOrigin() const
{
	return GridOrigin;
}

FIntPoint UTCFRTSPlacementGridSubsystem::WorldToGridCell(FVector WorldLocation) const
{
	if (!HasValidCellSize())
	{
		return FIntPoint::ZeroValue;
	}

	const FVector LocalLocation = WorldLocation - GridOrigin;

	return FIntPoint(
		FMath::FloorToInt(LocalLocation.X / CellSize),
		FMath::FloorToInt(LocalLocation.Y / CellSize));
}

FVector UTCFRTSPlacementGridSubsystem::GridCellToWorldCenter(FIntPoint GridCell) const
{
	if (!HasValidCellSize())
	{
		return GridOrigin;
	}

	return FVector(
		GridOrigin.X + (static_cast<float>(GridCell.X) + 0.5f) * CellSize,
		GridOrigin.Y + (static_cast<float>(GridCell.Y) + 0.5f) * CellSize,
		GridOrigin.Z);
}

FIntPoint UTCFRTSPlacementGridSubsystem::GetAnchorCellForWorldLocation(
	FVector WorldLocation,
	FIntPoint FootprintSize) const
{
	const FIntPoint SafeFootprintSize = GetSafeFootprintSize(FootprintSize);
	const FIntPoint CenterCell = WorldToGridCell(WorldLocation);

	return FIntPoint(
		CenterCell.X - SafeFootprintSize.X / 2,
		CenterCell.Y - SafeFootprintSize.Y / 2);
}

FVector UTCFRTSPlacementGridSubsystem::GetFootprintCenterWorldLocation(
	FIntPoint AnchorCell,
	FIntPoint FootprintSize) const
{
	if (!HasValidCellSize())
	{
		return GridOrigin;
	}

	const FIntPoint SafeFootprintSize = GetSafeFootprintSize(FootprintSize);

	return FVector(
		GridOrigin.X + (static_cast<float>(AnchorCell.X) + static_cast<float>(SafeFootprintSize.X) * 0.5f) * CellSize,
		GridOrigin.Y + (static_cast<float>(AnchorCell.Y) + static_cast<float>(SafeFootprintSize.Y) * 0.5f) * CellSize,
		GridOrigin.Z);
}

FVector UTCFRTSPlacementGridSubsystem::SnapWorldLocationToFootprintCenter(
	FVector WorldLocation,
	FIntPoint FootprintSize) const
{
	const FIntPoint AnchorCell = GetAnchorCellForWorldLocation(WorldLocation, FootprintSize);
	FVector SnappedLocation = GetFootprintCenterWorldLocation(AnchorCell, FootprintSize);
	SnappedLocation.Z = WorldLocation.Z;

	return SnappedLocation;
}

void UTCFRTSPlacementGridSubsystem::GetCellsForFootprint(
	FIntPoint AnchorCell,
	FIntPoint FootprintSize,
	TArray<FIntPoint>& OutCells) const
{
	OutCells.Reset();

	const FIntPoint SafeFootprintSize = GetSafeFootprintSize(FootprintSize);

	for (int32 X = 0; X < SafeFootprintSize.X; ++X)
	{
		for (int32 Y = 0; Y < SafeFootprintSize.Y; ++Y)
		{
			OutCells.Add(FIntPoint(AnchorCell.X + X, AnchorCell.Y + Y));
		}
	}
}

bool UTCFRTSPlacementGridSubsystem::ValidateFootprint(
	FIntPoint AnchorCell,
	FIntPoint FootprintSize,
	const AActor* IgnoredActor,
	FTCFPlacementGridValidationResult& OutResult)
{
	CompactInvalidReservations();

	if (!HasValidCellSize())
	{
		OutResult = FTCFPlacementGridValidationResult::Failure(
			ETCFPlacementGridValidationFailure::InvalidCellSize);

		return false;
	}

	const FIntPoint SafeFootprintSize = GetSafeFootprintSize(FootprintSize);
	if (SafeFootprintSize.X <= 0 || SafeFootprintSize.Y <= 0)
	{
		OutResult = FTCFPlacementGridValidationResult::Failure(
			ETCFPlacementGridValidationFailure::InvalidFootprint);

		return false;
	}

	TArray<FIntPoint> FootprintCells;
	GetCellsForFootprint(AnchorCell, SafeFootprintSize, FootprintCells);

	for (const FIntPoint& FootprintCell : FootprintCells)
	{
		AActor* Occupant = GetCellOccupant(FootprintCell);
		if (IsValid(Occupant) && Occupant != IgnoredActor)
		{
			OutResult = FTCFPlacementGridValidationResult::Failure(
				ETCFPlacementGridValidationFailure::CellOccupied,
				FootprintCell,
				Occupant);

			return false;
		}
	}

	OutResult = FTCFPlacementGridValidationResult::Success();
	return true;
}

bool UTCFRTSPlacementGridSubsystem::ValidateFootprintAtWorldLocation(
	FVector WorldLocation,
	FIntPoint FootprintSize,
	const AActor* IgnoredActor,
	FIntPoint& OutAnchorCell,
	FTCFPlacementGridValidationResult& OutResult)
{
	OutAnchorCell = GetAnchorCellForWorldLocation(WorldLocation, FootprintSize);

	return ValidateFootprint(
		OutAnchorCell,
		FootprintSize,
		IgnoredActor,
		OutResult);
}

bool UTCFRTSPlacementGridSubsystem::ReserveFootprint(
	AActor* OwnerActor,
	FIntPoint AnchorCell,
	FIntPoint FootprintSize,
	bool bReplaceExistingReservation,
	FTCFPlacementGridValidationResult& OutResult)
{
	if (!IsValid(OwnerActor))
	{
		OutResult = FTCFPlacementGridValidationResult::Failure(
			ETCFPlacementGridValidationFailure::InvalidFootprint);

		return false;
	}

	const AActor* IgnoredActor = bReplaceExistingReservation ? OwnerActor : nullptr;
	if (!ValidateFootprint(AnchorCell, FootprintSize, IgnoredActor, OutResult))
	{
		return false;
	}

	if (bReplaceExistingReservation)
	{
		ReleaseFootprintForActor(OwnerActor);
	}

	TArray<FIntPoint> FootprintCells;
	GetCellsForFootprint(AnchorCell, FootprintSize, FootprintCells);

	for (const FIntPoint& FootprintCell : FootprintCells)
	{
		OccupiedCells.Add(FootprintCell, OwnerActor);
	}

	FTCFPlacementGridReservation Reservation;
	Reservation.OwnerActor = OwnerActor;
	Reservation.AnchorCell = AnchorCell;
	Reservation.FootprintSize = GetSafeFootprintSize(FootprintSize);
	Reservation.OccupiedCells = FootprintCells;

	ActorReservations.Add(TWeakObjectPtr<AActor>(OwnerActor), Reservation);

	OutResult = FTCFPlacementGridValidationResult::Success();
	return true;
}

bool UTCFRTSPlacementGridSubsystem::ReserveFootprintAtWorldLocation(
	AActor* OwnerActor,
	FVector WorldLocation,
	FIntPoint FootprintSize,
	bool bReplaceExistingReservation,
	FIntPoint& OutAnchorCell,
	FTCFPlacementGridValidationResult& OutResult)
{
	OutAnchorCell = GetAnchorCellForWorldLocation(WorldLocation, FootprintSize);

	return ReserveFootprint(
		OwnerActor,
		OutAnchorCell,
		FootprintSize,
		bReplaceExistingReservation,
		OutResult);
}

void UTCFRTSPlacementGridSubsystem::ReleaseFootprintForActor(AActor* OwnerActor)
{
	if (!OwnerActor)
	{
		return;
	}

	for (auto ReservationIterator = ActorReservations.CreateIterator(); ReservationIterator; ++ReservationIterator)
	{
		if (ReservationIterator.Key().Get() != OwnerActor)
		{
			continue;
		}

		for (const FIntPoint& OccupiedCell : ReservationIterator.Value().OccupiedCells)
		{
			const TWeakObjectPtr<AActor>* Occupant = OccupiedCells.Find(OccupiedCell);
			if (Occupant && Occupant->Get() == OwnerActor)
			{
				OccupiedCells.Remove(OccupiedCell);
			}
		}

		ReservationIterator.RemoveCurrent();
		return;
	}
}

bool UTCFRTSPlacementGridSubsystem::IsCellOccupied(FIntPoint GridCell)
{
	CompactInvalidReservations();

	return IsValid(GetCellOccupant(GridCell));
}

AActor* UTCFRTSPlacementGridSubsystem::GetCellOccupant(FIntPoint GridCell)
{
	CompactInvalidReservations();

	const TWeakObjectPtr<AActor>* FoundOccupant = OccupiedCells.Find(GridCell);
	return FoundOccupant ? FoundOccupant->Get() : nullptr;
}

bool UTCFRTSPlacementGridSubsystem::GetReservedFootprintForActor(
	AActor* OwnerActor,
	FTCFPlacementGridReservation& OutReservation) const
{
	if (!OwnerActor)
	{
		return false;
	}

	for (const TPair<TWeakObjectPtr<AActor>, FTCFPlacementGridReservation>& ReservationPair : ActorReservations)
	{
		if (ReservationPair.Key.Get() == OwnerActor)
		{
			OutReservation = ReservationPair.Value;
			return true;
		}
	}

	return false;
}

FIntPoint UTCFRTSPlacementGridSubsystem::GetSafeFootprintSize(FIntPoint FootprintSize) const
{
	return FIntPoint(
		FMath::Max(1, FootprintSize.X),
		FMath::Max(1, FootprintSize.Y));
}

bool UTCFRTSPlacementGridSubsystem::HasValidCellSize() const
{
	return CellSize > KINDA_SMALL_NUMBER;
}

void UTCFRTSPlacementGridSubsystem::CompactInvalidReservations()
{
	for (auto CellIterator = OccupiedCells.CreateIterator(); CellIterator; ++CellIterator)
	{
		if (!CellIterator.Value().IsValid())
		{
			CellIterator.RemoveCurrent();
		}
	}

	for (auto ReservationIterator = ActorReservations.CreateIterator(); ReservationIterator; ++ReservationIterator)
	{
		if (!ReservationIterator.Key().IsValid())
		{
			ReservationIterator.RemoveCurrent();
		}
	}
}