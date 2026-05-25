//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFPlayerSelectionComponent.h"

#include "Actors/TCFSquadActor.h"
#include "Components/TCFSquadSelectionComponent.h"

UTCFPlayerSelectionComponent::UTCFPlayerSelectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

bool UTCFPlayerSelectionComponent::TrySelectSquad(ATCFSquadActor* Squad)
{
	if (!IsValid(Squad))
	{
		ClearSelection();
		return false;
	}

	if (SelectedSquad == Squad)
	{
		return true;
	}

	ClearSelection();

	SelectedSquad = Squad;
	SetSquadSelectedState(SelectedSquad, true);

	OnSelectedSquadChanged.Broadcast(SelectedSquad);

	return true;
}

void UTCFPlayerSelectionComponent::ClearSelection()
{
	if (SelectedSquad)
	{
		SetSquadSelectedState(SelectedSquad, false);
	}
	else
	{
		return;
	}

	SelectedSquad = nullptr;
	OnSelectedSquadChanged.Broadcast(nullptr);
}

ATCFSquadActor* UTCFPlayerSelectionComponent::GetSelectedSquad() const
{
	return IsValid(SelectedSquad) ? SelectedSquad : nullptr;
}

UAbilitySystemComponent* UTCFPlayerSelectionComponent::GetSelectedSquadAbilitySystem() const
{
	const ATCFSquadActor* Squad = GetSelectedSquad();
	return Squad ? Squad->GetAbilitySystemComponent() : nullptr;
}

void UTCFPlayerSelectionComponent::SetSquadSelectedState(ATCFSquadActor* Squad, bool bSelected) const
{
	if (!IsValid(Squad))
	{
		return;
	}

	UTCFSquadSelectionComponent* SelectionComponent = Squad->GetSquadSelectionComponent();
	if (!SelectionComponent)
	{
		return;
	}

	SelectionComponent->SetSelected(bSelected);
}
