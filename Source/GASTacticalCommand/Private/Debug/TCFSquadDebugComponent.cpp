//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Debug/TCFSquadDebugComponent.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "GAS/TCFSquadAttributeSet.h"
#include "GameFramework/Actor.h"
#include "TCFLogChannels.h"

UTCFSquadDebugComponent::UTCFSquadDebugComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFSquadDebugComponent::BeginPlay()
{
	Super::BeginPlay();

	ObservedSelectionComponent = FindSelectionComponent();
	if (!ObservedSelectionComponent)
	{
		UE_LOG(LogTCF, Warning, TEXT("TCF Debug: '%s' could not find a PlayerSelectionComponent to observe."), *GetName());
		return;
	}

	ObservedSelectionComponent->OnSelectedSquadChanged.AddDynamic(this, &UTCFSquadDebugComponent::HandleSelectedSquadChanged);
}

void UTCFSquadDebugComponent::LogSelectedSquadState() const
{
	if (!ObservedSelectionComponent)
	{
		UE_LOG(LogTCF, Log, TEXT("TCF Debug: No observed selection component."));
		return;
	}

	LogSquadState(ObservedSelectionComponent->GetSelectedSquad());
}

void UTCFSquadDebugComponent::LogSquadState(const ATCFSquadActor* Squad) const
{
	if (!IsValid(Squad))
	{
		UE_LOG(LogTCF, Log, TEXT("TCF Debug: No selected squad."));
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
		TEXT("TCF Debug Squad: %s | Initialized=%s | Role=%s | Tags=%s"),
		*Squad->GetName(),
		Squad->IsInitialized() ? TEXT("true") : TEXT("false"),
		*Squad->GetRoleTag().ToString(),
		*OwnedTags.ToStringSimple());

	if (!AttributeSet)
	{
		UE_LOG(LogTCF, Warning, TEXT("TCF Debug: Squad '%s' has no AttributeSet."), *Squad->GetName());
		return;
	}

	UE_LOG(LogTCF, Log,
		TEXT("TCF Debug Attributes: Morale=%.2f Suppression=%.2f Cohesion=%.2f Stamina=%.2f Accuracy=%.2f Defense=%.2f MoveSpeed=%.2f CapturePower=%.2f"),
		AttributeSet->GetMorale(),
		AttributeSet->GetSuppression(),
		AttributeSet->GetCohesion(),
		AttributeSet->GetStamina(),
		AttributeSet->GetAccuracy(),
		AttributeSet->GetDefense(),
		AttributeSet->GetMovementSpeed(),
		AttributeSet->GetCapturePower());
}

void UTCFSquadDebugComponent::HandleSelectedSquadChanged(ATCFSquadActor* SelectedSquad)
{
	if (!bLogSelectionChanges)
	{
		return;
	}

	LogSquadState(SelectedSquad);
}

UTCFPlayerSelectionComponent* UTCFSquadDebugComponent::FindSelectionComponent() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	return Owner->FindComponentByClass<UTCFPlayerSelectionComponent>();
}