//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFPlayerMovementCommandComponent.h"

#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Components/TCFSquadMovementComponent.h"

UTCFPlayerMovementCommandComponent::UTCFPlayerMovementCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFPlayerMovementCommandComponent::BeginPlay()
{
	Super::BeginPlay();

	SelectionComponent = GetOwner()
		? GetOwner()->FindComponentByClass<UTCFPlayerSelectionComponent>()
		: nullptr;
}

bool UTCFPlayerMovementCommandComponent::MoveSelectedSquadToLocation(FVector WorldLocation)
{
	return MoveSelectedSquadsToLocation(WorldLocation);
}

bool UTCFPlayerMovementCommandComponent::MoveSelectedSquadsToLocation(FVector WorldLocation)
{
	TArray<ATCFSquadActor*> SelectedSquads;
	GetSelectedSquads(SelectedSquads);

	if (SelectedSquads.IsEmpty())
	{
		return false;
	}

	bool bMovedAnySquad = false;

	for (int32 Index = 0; Index < SelectedSquads.Num(); ++Index)
	{
		ATCFSquadActor* Squad = SelectedSquads[Index];
		if (!IsValid(Squad))
		{
			continue;
		}

		UTCFSquadMovementComponent* SquadMovementComponent = Squad->GetMovementComponent();
		if (!SquadMovementComponent)
		{
			continue;
		}

		const FVector TargetLocation = WorldLocation + GetFormationOffset(Index, SelectedSquads.Num());
		bMovedAnySquad |= SquadMovementComponent->MoveToLocation(TargetLocation);
	}

	return bMovedAnySquad;
}

void UTCFPlayerMovementCommandComponent::StopSelectedSquadMovement()
{
	StopSelectedSquadsMovement();
}

void UTCFPlayerMovementCommandComponent::StopSelectedSquadsMovement()
{
	TArray<ATCFSquadActor*> SelectedSquads;
	GetSelectedSquads(SelectedSquads);

	for (ATCFSquadActor* Squad : SelectedSquads)
	{
		if (!IsValid(Squad))
		{
			continue;
		}

		if (UTCFSquadMovementComponent* SquadMovementComponent = Squad->GetMovementComponent())
		{
			SquadMovementComponent->StopMovement(false);
		}
	}
}

ATCFSquadActor* UTCFPlayerMovementCommandComponent::GetSelectedSquad() const
{
	return SelectionComponent ? SelectionComponent->GetSelectedSquad() : nullptr;
}

void UTCFPlayerMovementCommandComponent::GetSelectedSquads(TArray<ATCFSquadActor*>& OutSelectedSquads) const
{
	OutSelectedSquads.Reset();

	if (SelectionComponent)
	{
		SelectionComponent->GetSelectedSquads(OutSelectedSquads);
	}
}

FVector UTCFPlayerMovementCommandComponent::GetFormationOffset(int32 Index, int32 Count) const
{
	if (Count <= 1 || MultiMoveSpacing <= 0.0f)
	{
		return FVector::ZeroVector;
	}

	const int32 Columns = FMath::CeilToInt(FMath::Sqrt(static_cast<float>(Count)));
	const int32 Row = Index / Columns;
	const int32 Column = Index % Columns;

	const float HalfWidth = static_cast<float>(Columns - 1) * 0.5f;
	const int32 Rows = FMath::CeilToInt(static_cast<float>(Count) / static_cast<float>(Columns));
	const float HalfHeight = static_cast<float>(Rows - 1) * 0.5f;

	return FVector(
		(static_cast<float>(Column) - HalfWidth) * MultiMoveSpacing,
		(static_cast<float>(Row) - HalfHeight) * MultiMoveSpacing,
		0.0f);
}