//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFPlayerSelectionComponent.h"

#include "AbilitySystemComponent.h"
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

	ClearSelection();

	AddSquadInternal(Squad);
	SetPrimarySelectedSquad(Squad);
	BroadcastSelectionChanged();

	return true;
}

bool UTCFPlayerSelectionComponent::AddSquadToSelection(ATCFSquadActor* Squad)
{
	if (!IsValid(Squad))
	{
		return false;
	}

	CompactSelection();

	if (SelectedSquads.Contains(Squad))
	{
		SetPrimarySelectedSquad(Squad);
		BroadcastSelectionChanged();
		return true;
	}

	AddSquadInternal(Squad);

	if (!IsValid(PrimarySelectedSquad))
	{
		SetPrimarySelectedSquad(Squad);
	}
	else
	{
		SetPrimarySelectedSquad(Squad);
	}

	BroadcastSelectionChanged();
	return true;
}

bool UTCFPlayerSelectionComponent::RemoveSquadFromSelection(ATCFSquadActor* Squad)
{
	if (!IsValid(Squad))
	{
		return false;
	}

	CompactSelection();

	if (!SelectedSquads.Contains(Squad))
	{
		return false;
	}

	RemoveSquadInternal(Squad);

	if (PrimarySelectedSquad == Squad)
	{
		SetPrimarySelectedSquad(SelectedSquads.Num() > 0 ? SelectedSquads.Last() : nullptr);
	}

	BroadcastSelectionChanged();
	return true;
}

bool UTCFPlayerSelectionComponent::ToggleSquadSelection(ATCFSquadActor* Squad)
{
	if (!IsValid(Squad))
	{
		return false;
	}

	CompactSelection();

	return SelectedSquads.Contains(Squad)
		? RemoveSquadFromSelection(Squad)
		: AddSquadToSelection(Squad);
}

void UTCFPlayerSelectionComponent::SetSelectedSquads(const TArray<ATCFSquadActor*>& Squads, bool bAppendSelection)
{
	if (!bAppendSelection)
	{
		ClearSelection();
	}
	else
	{
		CompactSelection();
	}

	ATCFSquadActor* LastValidSquad = nullptr;

	for (ATCFSquadActor* Squad : Squads)
	{
		if (!IsValid(Squad) || SelectedSquads.Contains(Squad))
		{
			continue;
		}

		AddSquadInternal(Squad);
		LastValidSquad = Squad;
	}

	if (LastValidSquad)
	{
		SetPrimarySelectedSquad(LastValidSquad);
	}
	else if (!IsValid(PrimarySelectedSquad) && SelectedSquads.Num() > 0)
	{
		SetPrimarySelectedSquad(SelectedSquads.Last());
	}

	BroadcastSelectionChanged();
}

void UTCFPlayerSelectionComponent::ClearSelection()
{
	CompactSelection();

	for (ATCFSquadActor* Squad : SelectedSquads)
	{
		SetSquadSelectedState(Squad, false);
	}

	SelectedSquads.Reset();
	SetPrimarySelectedSquad(nullptr);
	BroadcastSelectionChanged();
}

ATCFSquadActor* UTCFPlayerSelectionComponent::GetSelectedSquad() const
{
	return GetPrimarySelectedSquad();
}

ATCFSquadActor* UTCFPlayerSelectionComponent::GetPrimarySelectedSquad() const
{
	return IsValid(PrimarySelectedSquad) ? PrimarySelectedSquad : nullptr;
}

void UTCFPlayerSelectionComponent::GetSelectedSquads(TArray<ATCFSquadActor*>& OutSelectedSquads) const
{
	OutSelectedSquads.Reset();

	for (ATCFSquadActor* Squad : SelectedSquads)
	{
		if (IsValid(Squad))
		{
			OutSelectedSquads.Add(Squad);
		}
	}
}

int32 UTCFPlayerSelectionComponent::GetSelectedSquadCount() const
{
	int32 Count = 0;

	for (const ATCFSquadActor* Squad : SelectedSquads)
	{
		if (IsValid(Squad))
		{
			Count++;
		}
	}

	return Count;
}

bool UTCFPlayerSelectionComponent::HasSelectedSquads() const
{
	return GetSelectedSquadCount() > 0;
}

UAbilitySystemComponent* UTCFPlayerSelectionComponent::GetSelectedSquadAbilitySystem() const
{
	const ATCFSquadActor* Squad = GetPrimarySelectedSquad();
	return Squad ? Squad->GetAbilitySystemComponent() : nullptr;
}

void UTCFPlayerSelectionComponent::AddSquadInternal(ATCFSquadActor* Squad)
{
	if (!IsValid(Squad))
	{
		return;
	}

	SelectedSquads.Add(Squad);
	SetSquadSelectedState(Squad, true);
}

void UTCFPlayerSelectionComponent::RemoveSquadInternal(ATCFSquadActor* Squad)
{
	if (!IsValid(Squad))
	{
		return;
	}

	SelectedSquads.Remove(Squad);
	SetSquadSelectedState(Squad, false);
}

void UTCFPlayerSelectionComponent::CompactSelection()
{
	for (int32 Index = SelectedSquads.Num() - 1; Index >= 0; --Index)
	{
		if (!IsValid(SelectedSquads[Index]))
		{
			SelectedSquads.RemoveAtSwap(Index);
		}
	}

	if (!IsValid(PrimarySelectedSquad) && SelectedSquads.Num() > 0)
	{
		PrimarySelectedSquad = SelectedSquads.Last();
	}
}

void UTCFPlayerSelectionComponent::SetPrimarySelectedSquad(ATCFSquadActor* Squad)
{
	PrimarySelectedSquad = IsValid(Squad) ? Squad : nullptr;
}

void UTCFPlayerSelectionComponent::BroadcastSelectionChanged() const
{
	OnSelectedSquadChanged.Broadcast(GetPrimarySelectedSquad());
	OnSelectionCountChanged.Broadcast(GetSelectedSquadCount());
}

void UTCFPlayerSelectionComponent::SetSquadSelectedState(const ATCFSquadActor* Squad, const bool bSelected)
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