//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFPlayerOrderComponent.h"

#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Subsystems/TCFOrderSubsystem.h"
#include "TCFGameplayTags.h"

UTCFPlayerOrderComponent::UTCFPlayerOrderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFPlayerOrderComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerSelectionComponent = FindSelectionComponent();
}

bool UTCFPlayerOrderComponent::SubmitSelectedSquadOrder(
	FGameplayTag OrderTag,
	const FTCFOrderTarget& Target,
	FTCFOrderResult& OutResult)
{
	if (!PlayerSelectionComponent)
	{
		PlayerSelectionComponent = FindSelectionComponent();
	}

	if (!PlayerSelectionComponent)
	{
		OutResult = FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "MissingPlayerSelectionComponent", "Player order component could not find a selection component."));

		return false;
	}

	ATCFSquadActor* SelectedSquad = PlayerSelectionComponent->GetSelectedSquad();
	if (!IsValid(SelectedSquad))
	{
		OutResult = FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "NoSelectedSquadForOrder", "No squad is selected for this order."));

		return false;
	}

	UWorld* World = GetWorld();
	UTCFOrderSubsystem* OrderSubsystem = World ? World->GetSubsystem<UTCFOrderSubsystem>() : nullptr;
	if (!OrderSubsystem)
	{
		OutResult = FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_MissingDefinition,
			NSLOCTEXT("TCFOrders", "MissingOrderSubsystem", "Order subsystem is not available."));

		return false;
	}

	FTCFSquadOrderRequest Request;
	Request.OrderTag = OrderTag;
	Request.SourceActor = SelectedSquad;
	Request.Target = Target;
	Request.OrderSequence = NextOrderSequence++;

	OutResult = OrderSubsystem->SubmitOrder(Request);

	OnOrderSubmitted.Broadcast(Request, OutResult);

	return OutResult.bSuccess;
}

UTCFPlayerSelectionComponent* UTCFPlayerOrderComponent::FindSelectionComponent() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	return Owner->FindComponentByClass<UTCFPlayerSelectionComponent>();
}
