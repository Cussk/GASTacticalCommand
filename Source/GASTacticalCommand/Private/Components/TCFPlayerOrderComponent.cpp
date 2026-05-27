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

	return SubmitSquadOrder(*OrderSubsystem, SelectedSquad, OrderTag, Target, OutResult);
}

bool UTCFPlayerOrderComponent::SubmitSelectedSquadsOrder(
	FGameplayTag OrderTag,
	const FTCFOrderTarget& Target,
	TArray<FTCFOrderResult>& OutResults)
{
	OutResults.Reset();

	if (!PlayerSelectionComponent)
	{
		PlayerSelectionComponent = FindSelectionComponent();
	}

	if (!PlayerSelectionComponent)
	{
		OutResults.Add(FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "MissingPlayerSelectionComponentBatch", "Player order component could not find a selection component.")));

		return false;
	}

	TArray<ATCFSquadActor*> SelectedSquads;
	PlayerSelectionComponent->GetSelectedSquads(SelectedSquads);

	if (SelectedSquads.IsEmpty())
	{
		OutResults.Add(FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "NoSelectedSquadsForOrder", "No squads are selected for this order.")));

		return false;
	}

	UWorld* World = GetWorld();
	UTCFOrderSubsystem* OrderSubsystem = World ? World->GetSubsystem<UTCFOrderSubsystem>() : nullptr;
	if (!OrderSubsystem)
	{
		OutResults.Add(FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_MissingDefinition,
			NSLOCTEXT("TCFOrders", "MissingOrderSubsystemBatch", "Order subsystem is not available.")));

		return false;
	}

	bool bAnyOrderSucceeded = false;

	for (ATCFSquadActor* SelectedSquad : SelectedSquads)
	{
		if (!IsValid(SelectedSquad))
		{
			continue;
		}

		FTCFOrderResult SquadResult;
		const bool bSubmitted = SubmitSquadOrder(
			*OrderSubsystem,
			SelectedSquad,
			OrderTag,
			Target,
			SquadResult);

		OutResults.Add(SquadResult);
		bAnyOrderSucceeded |= bSubmitted;
	}

	return bAnyOrderSucceeded;
}

bool UTCFPlayerOrderComponent::SubmitSquadOrder(
	UTCFOrderSubsystem& OrderSubsystem,
	ATCFSquadActor* SourceSquad,
	FGameplayTag OrderTag,
	const FTCFOrderTarget& Target,
	FTCFOrderResult& OutResult)
{
	if (!IsValid(SourceSquad))
	{
		OutResult = FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "InvalidSquadForOrder", "The squad source for this order is invalid."));

		return false;
	}

	FTCFSquadOrderRequest Request;
	Request.OrderTag = OrderTag;
	Request.SourceActor = SourceSquad;
	Request.Target = Target;
	Request.OrderSequence = NextOrderSequence++;

	OutResult = OrderSubsystem.SubmitOrder(Request);

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
