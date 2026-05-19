//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFSquadSelectionComponent.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

UTCFSquadSelectionComponent::UTCFSquadSelectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFSquadSelectionComponent::BeginPlay()
{
	Super::BeginPlay();

	ApplySelectionVisualState();
}

void UTCFSquadSelectionComponent::SetSelected(bool bNewSelected)
{
	if (bSelected == bNewSelected)
	{
		return;
	}

	bSelected = bNewSelected;
	ApplySelectionVisualState();

	OnSelectionChanged.Broadcast(bSelected);
}

bool UTCFSquadSelectionComponent::IsSelected() const
{
	return bSelected;
}

void UTCFSquadSelectionComponent::ApplySelectionVisualState() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent)
		{
			continue;
		}

		PrimitiveComponent->SetRenderCustomDepth(bSelected);
	}
}