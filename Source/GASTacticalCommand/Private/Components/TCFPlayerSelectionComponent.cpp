//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFPlayerSelectionComponent.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFBuildingActor.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFSelectableHighlightComponent.h"
#include "Player/TCFPlayerState.h"
#include "Subsystems/TCFRelationshipSubsystem.h"

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
		ClearInspectedBuilding();
		ClearInspectedSquad();
		return false;
	}

	if (!IsOwnSquad(Squad))
	{
		return TryInspectSquad(Squad);
	}

	ClearInspectedBuilding();
	ClearInspectedSquad();
	ClearSelection();

	AddSquadInternal(Squad);
	SetPrimarySelectedSquad(Squad);
	BroadcastSelectionChanged();

	return true;
}

bool UTCFPlayerSelectionComponent::AddSquadToSelection(ATCFSquadActor* Squad)
{
	if (!IsValid(Squad) || !IsOwnSquad(Squad))
	{
		return false;
	}
	
	ClearInspectedBuilding();
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
	if (!IsValid(Squad) || !IsOwnSquad(Squad))
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

	bool bHasOwnIncomingSquad = false;

	for (ATCFSquadActor* Squad : Squads)
	{
		if (!IsValid(Squad) || !IsOwnSquad(Squad))
		{
			continue;
		}

		bHasOwnIncomingSquad = true;

		if (SelectedSquads.Contains(Squad))
		{
			continue;
		}

		AddSquadInternal(Squad);
		LastValidSquad = Squad;
	}

	if (bHasOwnIncomingSquad)
	{
		ClearInspectedBuilding();
		ClearInspectedSquad();
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

	ClearInspectedBuilding();
	ClearInspectedSquad();
	
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

bool UTCFPlayerSelectionComponent::IsOwnSquad(const ATCFSquadActor* Squad) const
{
	if (!IsValid(Squad))
	{
		return false;
	}

	const UWorld* World = GetWorld();
	const UTCFRelationshipSubsystem* RelationshipSubsystem = World ? World->GetSubsystem<UTCFRelationshipSubsystem>() : nullptr;

	const APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	const ATCFPlayerState* TCFPlayerState = PlayerController ? Cast<ATCFPlayerState>(PlayerController->PlayerState) : nullptr;

	if (!RelationshipSubsystem || !TCFPlayerState)
	{
		return false;
	}

	return RelationshipSubsystem->GetActorRelationship(TCFPlayerState, Squad) == ETCFSquadRelationship::Own;
}

void UTCFPlayerSelectionComponent::SetSquadSelectedState(const ATCFSquadActor* Squad, const bool bSelected)
{
	if (!IsValid(Squad))
	{
		return;
	}

	UTCFSelectableHighlightComponent* SelectableHighlightComponent = Squad->GetSelectableHighlightComponent();
	if (!SelectableHighlightComponent)
	{
		return;
	}

	SelectableHighlightComponent->SetSelected(bSelected);
}

void UTCFPlayerSelectionComponent::SetInspectedSquad(ATCFSquadActor* Squad)
{
	ATCFSquadActor* NewSquad = IsValid(Squad) ? Squad : nullptr;
	if (InspectedSquad == NewSquad)
	{
		return;
	}

	if (UTCFSelectableHighlightComponent* OldHighlight =
		InspectedSquad ? InspectedSquad->FindComponentByClass<UTCFSelectableHighlightComponent>() : nullptr)
	{
		OldHighlight->SetSelected(false);
	}

	InspectedSquad = NewSquad;

	if (UTCFSelectableHighlightComponent* NewHighlight =
		InspectedSquad ? InspectedSquad->FindComponentByClass<UTCFSelectableHighlightComponent>() : nullptr)
	{
		NewHighlight->SetSelected(true);
	}

	OnInspectedSquadChanged.Broadcast(GetInspectedSquad());
}

bool UTCFPlayerSelectionComponent::TryInspectSquad(ATCFSquadActor* Squad)
{
	if (!IsValid(Squad))
	{
		ClearSelection();
		ClearInspectedSquad();
		return false;
	}

	ClearSelection();
	ClearInspectedBuilding();
	SetInspectedSquad(Squad);
	return true;
}

void UTCFPlayerSelectionComponent::ClearInspectedSquad()
{
	SetInspectedSquad(nullptr);
}

ATCFSquadActor* UTCFPlayerSelectionComponent::GetInspectedSquad() const
{
	return InspectedSquad;
}

bool UTCFPlayerSelectionComponent::HasInspectedSquad() const
{
	return IsValid(InspectedSquad);
}

bool UTCFPlayerSelectionComponent::TryInspectBuilding(ATCFBuildingActor* Building)
{
	if (!IsValid(Building) || !Building->IsAlive())
	{
		ClearSelection();
		ClearInspectedBuilding();
		return false;
	}

	ClearSelection();
	SetInspectedBuilding(Building);
	return true;
}

void UTCFPlayerSelectionComponent::ClearInspectedBuilding()
{
	SetInspectedBuilding(nullptr);
}

ATCFBuildingActor* UTCFPlayerSelectionComponent::GetInspectedBuilding() const
{
	return IsValid(InspectedBuilding) ? InspectedBuilding : nullptr;
}

bool UTCFPlayerSelectionComponent::HasInspectedBuilding() const
{
	return IsValid(InspectedBuilding);
}

UAbilitySystemComponent* UTCFPlayerSelectionComponent::GetInspectedBuildingAbilitySystem() const
{
	const ATCFBuildingActor* Building = GetInspectedBuilding();
	return Building ? Building->GetAbilitySystemComponent() : nullptr;
}

void UTCFPlayerSelectionComponent::SetInspectedBuilding(ATCFBuildingActor* Building)
{
	ATCFBuildingActor* NewBuilding = IsValid(Building) ? Building : nullptr;
	if (InspectedBuilding == NewBuilding)
	{
		return;
	}

	if (UTCFSelectableHighlightComponent* OldHighlight =
		InspectedBuilding ? InspectedBuilding->FindComponentByClass<UTCFSelectableHighlightComponent>() : nullptr)
	{
		OldHighlight->SetSelected(false);
	}

	UnbindInspectedBuildingDestroyed();

	InspectedBuilding = NewBuilding;

	if (InspectedBuilding)
	{
		BindInspectedBuildingDestroyed(InspectedBuilding);

		if (UTCFSelectableHighlightComponent* NewHighlight =
			InspectedBuilding->FindComponentByClass<UTCFSelectableHighlightComponent>())
		{
			NewHighlight->SetSelected(true);
		}
	}

	OnInspectedBuildingChanged.Broadcast(GetInspectedBuilding());
}

void UTCFPlayerSelectionComponent::BindInspectedBuildingDestroyed(ATCFBuildingActor* Building)
{
	if (IsValid(Building))
	{
		Building->OnDestroyed.AddDynamic(this, &UTCFPlayerSelectionComponent::HandleInspectedBuildingDestroyed);
	}
}

void UTCFPlayerSelectionComponent::UnbindInspectedBuildingDestroyed()
{
	if (IsValid(InspectedBuilding))
	{
		InspectedBuilding->OnDestroyed.RemoveDynamic(this, &UTCFPlayerSelectionComponent::HandleInspectedBuildingDestroyed);
	}
}

void UTCFPlayerSelectionComponent::HandleInspectedBuildingDestroyed(AActor* DestroyedActor)
{
	if (DestroyedActor == InspectedBuilding)
	{
		ClearInspectedBuilding();
	}
}