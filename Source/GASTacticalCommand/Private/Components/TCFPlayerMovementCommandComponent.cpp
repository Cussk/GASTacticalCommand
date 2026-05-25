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
	ATCFSquadActor* SelectedSquad = GetSelectedSquad();
	if (!IsValid(SelectedSquad))
	{
		return false;
	}

	UTCFSquadMovementComponent* SquadMovementComponent = SelectedSquad->GetMovementComponent();
	if (!SquadMovementComponent)
	{
		return false;
	}

	return SquadMovementComponent->MoveToLocation(WorldLocation);
}

void UTCFPlayerMovementCommandComponent::StopSelectedSquadMovement()
{
	ATCFSquadActor* SelectedSquad = GetSelectedSquad();
	if (!IsValid(SelectedSquad))
	{
		return;
	}

	if (UTCFSquadMovementComponent* SquadMovementComponent = SelectedSquad->GetMovementComponent())
	{
		SquadMovementComponent->StopMovement(false);
	}
}

ATCFSquadActor* UTCFPlayerMovementCommandComponent::GetSelectedSquad() const
{
	return SelectionComponent ? SelectionComponent->GetSelectedSquad() : nullptr;
}