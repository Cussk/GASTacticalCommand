//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFPlayerSelectionComponent.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFSquadSelectionComponent.h"
#include "GAS/TCFSquadAttributeSet.h"
#include "TCFLogChannels.h"

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
		if (bLogSelectionChanges)
		{
			LogSelectedSquadState();
		}

		return true;
	}

	ClearSelection();

	SelectedSquad = Squad;
	SetSquadSelectedState(SelectedSquad, true);

	OnSelectedSquadChanged.Broadcast(SelectedSquad);

	if (bLogSelectionChanges)
	{
		LogSelectedSquadState();
	}

	return true;
}

void UTCFPlayerSelectionComponent::ClearSelection()
{
	if (SelectedSquad)
	{
		SetSquadSelectedState(SelectedSquad, false);
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

void UTCFPlayerSelectionComponent::LogSelectedSquadState() const
{
	const ATCFSquadActor* Squad = GetSelectedSquad();
	if (!Squad)
	{
		UE_LOG(LogTCF, Log, TEXT("TCF Selection: No selected squad."));
		return;
	}

	const UTCFSquadAttributeSet* AttributeSet = Squad->GetSquadAttributeSet();
	const UAbilitySystemComponent* AbilitySystem = Squad->GetAbilitySystemComponent();

	FGameplayTagContainer OwnedTags;
	if (AbilitySystem)
	{
		AbilitySystem->GetOwnedGameplayTags(OwnedTags);
	}

	UE_LOG(LogTCF, Log,
		TEXT("TCF Selection: %s | Initialized=%s | Role=%s | Tags=%s"),
		*Squad->GetName(),
		Squad->IsInitialized() ? TEXT("true") : TEXT("false"),
		*Squad->GetRoleTag().ToString(),
		*OwnedTags.ToStringSimple());

	if (!AttributeSet)
	{
		UE_LOG(LogTCF, Warning, TEXT("TCF Selection: Selected squad has no AttributeSet."));
		return;
	}

	UE_LOG(LogTCF, Log,
		TEXT("TCF Attributes: Morale=%.2f Suppression=%.2f Cohesion=%.2f Stamina=%.2f Accuracy=%.2f Defense=%.2f MoveSpeed=%.2f CapturePower=%.2f"),
		AttributeSet->GetMorale(),
		AttributeSet->GetSuppression(),
		AttributeSet->GetCohesion(),
		AttributeSet->GetStamina(),
		AttributeSet->GetAccuracy(),
		AttributeSet->GetDefense(),
		AttributeSet->GetMovementSpeed(),
		AttributeSet->GetCapturePower());
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