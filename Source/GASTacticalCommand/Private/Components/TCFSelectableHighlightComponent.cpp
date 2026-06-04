//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFSelectableHighlightComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Components/TCFAffiliationComponent.h"
#include "GameFramework/PlayerController.h"
#include "GASTacticalCommand/GASTacticalCommand.h"
#include "Player/TCFPlayerState.h"
#include "Subsystems/TCFRelationshipSubsystem.h"

UTCFSelectableHighlightComponent::UTCFSelectableHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFSelectableHighlightComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerAffiliationComponent = GetOwner() ? GetOwner()->FindComponentByClass<UTCFAffiliationComponent>() : nullptr;

	BindAffiliationChanges();
	ApplySelectionVisualState();
}

void UTCFSelectableHighlightComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindAffiliationChanges();

	Super::EndPlay(EndPlayReason);
}

void UTCFSelectableHighlightComponent::SetSelected(bool bNewSelected)
{
	if (bSelected == bNewSelected)
	{
		return;
	}

	bSelected = bNewSelected;
	ApplySelectionVisualState();

	OnSelectionChanged.Broadcast(bSelected);
}

bool UTCFSelectableHighlightComponent::IsSelected() const
{
	return bSelected;
}

void UTCFSelectableHighlightComponent::ApplySelectionVisualState() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	const int32 StencilValue = ResolveStencilValueForLocalPlayer();

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent)
		{
			continue;
		}

		PrimitiveComponent->SetCustomDepthStencilValue(StencilValue);
		PrimitiveComponent->SetRenderCustomDepth(bSelected);
	}
}

int32 UTCFSelectableHighlightComponent::ResolveStencilValueForLocalPlayer() const
{
	switch (ResolveRelationshipToLocalPlayer())
	{
	case ETCFSquadRelationship::Own:
		return CUSTOM_DEPTH_GREEN;

	case ETCFSquadRelationship::Friendly:
		return CUSTOM_DEPTH_BLUE;

	case ETCFSquadRelationship::Enemy:
		return CUSTOM_DEPTH_RED;

	case ETCFSquadRelationship::Neutral:
	default:
		return CUSTOM_DEPTH_YELLOW;
	}
}

ETCFSquadRelationship UTCFSelectableHighlightComponent::ResolveRelationshipToLocalPlayer() const
{
	const AActor* Owner = GetOwner();
	const UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return ETCFSquadRelationship::Neutral;
	}

	const APlayerController* PlayerController = World->GetFirstPlayerController();
	const ATCFPlayerState* TCFPlayerState = PlayerController
		? Cast<ATCFPlayerState>(PlayerController->PlayerState)
		: nullptr;

	const UTCFRelationshipSubsystem* RelationshipSubsystem = World->GetSubsystem<UTCFRelationshipSubsystem>();
	if (!TCFPlayerState || !RelationshipSubsystem)
	{
		return ETCFSquadRelationship::Neutral;
	}

	return RelationshipSubsystem->GetActorRelationship(TCFPlayerState, Owner);
}

void UTCFSelectableHighlightComponent::BindAffiliationChanges()
{
	if (OwnerAffiliationComponent)
	{
		OwnerAffiliationComponent->OnAffiliationChanged.AddDynamic(this, &UTCFSelectableHighlightComponent::HandleOwnerAffiliationChanged);
	}
}

void UTCFSelectableHighlightComponent::UnbindAffiliationChanges()
{
	if (OwnerAffiliationComponent)
	{
		OwnerAffiliationComponent->OnAffiliationChanged.RemoveDynamic(this, &UTCFSelectableHighlightComponent::HandleOwnerAffiliationChanged);
	}
}

void UTCFSelectableHighlightComponent::HandleOwnerAffiliationChanged(const FTCFAffiliationData& NewAffiliation)
{
	ApplySelectionVisualState();
}