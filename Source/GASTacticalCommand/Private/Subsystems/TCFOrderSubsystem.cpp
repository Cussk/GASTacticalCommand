//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Subsystems/TCFOrderSubsystem.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFSquadActor.h"
#include "Data/TCFOrderDefinition.h"
#include "Settings/TCFOrderSettings.h"
#include "TCFGameplayTags.h"
#include "GAS/TCFOrderPayload.h"

void UTCFOrderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	RebuildOrderDefinitionCache();
}

FTCFOrderResult UTCFOrderSubsystem::SubmitOrder(const FTCFSquadOrderRequest& Request)
{
	const UTCFOrderDefinition* OrderDefinition = nullptr;
	const FTCFOrderResult ValidationResult = ValidateOrderRequest(Request, OrderDefinition);

	if (!ValidationResult.bSuccess)
	{
		return ValidationResult;
	}

	check(OrderDefinition);

	return ExecuteValidatedOrder(Request, *OrderDefinition);
}

const UTCFOrderDefinition* UTCFOrderSubsystem::GetOrderDefinition(FGameplayTag OrderTag) const
{
	if (!OrderTag.IsValid())
	{
		return nullptr;
	}

	const TObjectPtr<UTCFOrderDefinition>* FoundDefinition = OrderDefinitionsByTag.Find(OrderTag);
	return FoundDefinition ? FoundDefinition->Get() : nullptr;
}

void UTCFOrderSubsystem::RebuildOrderDefinitionCache()
{
	OrderDefinitionsByTag.Reset();

	const UTCFOrderSettings* Settings = GetDefault<UTCFOrderSettings>();
	if (!Settings)
	{
		return;
	}

	for (const TSoftObjectPtr<UTCFOrderDefinition>& DefinitionPtr : Settings->OrderDefinitions)
	{
		UTCFOrderDefinition* Definition = DefinitionPtr.LoadSynchronous();
		if (!Definition || !Definition->IsValidDefinition())
		{
			continue;
		}

		OrderDefinitionsByTag.Add(Definition->OrderTag, Definition);
	}
}

FTCFOrderResult UTCFOrderSubsystem::ValidateOrderRequest(
	const FTCFSquadOrderRequest& Request,
	const UTCFOrderDefinition*& OutOrderDefinition) const
{
	OutOrderDefinition = nullptr;

	if (!Request.IsValidRequest())
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "InvalidOrderRequest", "Order request is missing a valid order tag or source actor."));
	}

	const UTCFOrderDefinition* OrderDefinition = GetOrderDefinition(Request.OrderTag);
	if (!OrderDefinition)
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_MissingDefinition,
			NSLOCTEXT("TCFOrders", "MissingOrderDefinition", "No order definition was registered for this order tag."));
	}

	const ATCFSquadActor* SourceSquad = Cast<ATCFSquadActor>(Request.SourceActor);
	if (!IsValid(SourceSquad))
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "InvalidSourceSquad", "Order source is not a valid squad actor."));
	}

	if (!SourceSquad->IsInitialized())
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "UninitializedSourceSquad", "Order source squad is not initialized."));
	}

	const FTCFOrderResult SourceTagResult = ValidateSourceTags(*SourceSquad, *OrderDefinition);
	if (!SourceTagResult.bSuccess)
	{
		return SourceTagResult;
	}

	const FTCFOrderResult TargetResult = ValidateTarget(*Request.SourceActor, Request.Target, *OrderDefinition);
	if (!TargetResult.bSuccess)
	{
		return TargetResult;
	}

	OutOrderDefinition = OrderDefinition;

	return FTCFOrderResult::Success(TCFGameplayTags::Order_Result_Success);
}

FTCFOrderResult UTCFOrderSubsystem::ValidateSourceTags(
	const ATCFSquadActor& SourceSquad,
	const UTCFOrderDefinition& OrderDefinition) const
{
	const UAbilitySystemComponent* AbilitySystem = SourceSquad.GetAbilitySystemComponent();
	if (!AbilitySystem)
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "MissingSourceASC", "Source squad has no Ability System Component."));
	}

	FGameplayTagContainer OwnedTags;
	AbilitySystem->GetOwnedGameplayTags(OwnedTags);

	FGameplayTagContainer MissingTags;
	if (!DoesSourceHaveRequiredTags(OwnedTags, OrderDefinition.RequiredSourceTags, MissingTags))
	{
		return FTCFOrderResult::FailureWithBlockingTags(
			TCFGameplayTags::Order_Result_Failed_MissingRequirements,
			MissingTags,
			NSLOCTEXT("TCFOrders", "MissingRequiredSourceTags", "Source squad is missing required tags for this order."));
	}

	FGameplayTagContainer BlockingTags;
	if (DoesSourceHaveBlockedTags(OwnedTags, OrderDefinition.BlockedSourceTags, BlockingTags))
	{
		return FTCFOrderResult::FailureWithBlockingTags(
			TCFGameplayTags::Order_Result_Failed_BlockedByTags,
			BlockingTags,
			NSLOCTEXT("TCFOrders", "BlockedSourceTags", "Source squad has tags that block this order."));
	}

	return FTCFOrderResult::Success(TCFGameplayTags::Order_Result_Success);
}

FTCFOrderResult UTCFOrderSubsystem::ValidateTarget(
	const AActor& SourceActor,
	const FTCFOrderTarget& Target,
	const UTCFOrderDefinition& OrderDefinition) const
{
	if (!IsTargetTypeValid(Target, OrderDefinition))
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidTarget,
			NSLOCTEXT("TCFOrders", "InvalidOrderTarget", "Order target does not match the order definition."));
	}

	if (!IsTargetWithinRange(SourceActor, Target, OrderDefinition))
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_OutOfRange,
			NSLOCTEXT("TCFOrders", "OrderTargetOutOfRange", "Order target is out of range."));
	}

	return FTCFOrderResult::Success(TCFGameplayTags::Order_Result_Success);
}

FTCFOrderResult UTCFOrderSubsystem::ExecuteValidatedOrder(
	const FTCFSquadOrderRequest& Request,
	const UTCFOrderDefinition& OrderDefinition) const
{
	ATCFSquadActor* SourceSquad = Cast<ATCFSquadActor>(Request.SourceActor);
	if (!IsValid(SourceSquad))
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "ExecutionInvalidSourceSquad", "Order execution source is not a valid squad actor."));
	}

	if (!SourceSquad->HasAuthority())
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_NoAuthority,
			NSLOCTEXT("TCFOrders", "OrderExecutionNoAuthority", "Only the authority can execute squad orders."));
	}

	return TriggerOrderAbility(*SourceSquad, OrderDefinition, Request);
}

FTCFOrderResult UTCFOrderSubsystem::TriggerOrderAbility(
	const ATCFSquadActor& SourceSquad,
	const UTCFOrderDefinition& OrderDefinition,
	const FTCFSquadOrderRequest& Request) const
{
	if (!OrderDefinition.HasAbilityClass())
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_MissingAbility,
			NSLOCTEXT("TCFOrders", "OrderDefinitionMissingAbility", "Order definition does not have an ability class assigned."));
	}

	UAbilitySystemComponent* AbilitySystem = SourceSquad.GetAbilitySystemComponent();
	if (!AbilitySystem)
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "OrderExecutionMissingASC", "Source squad has no Ability System Component."));
	}

	FGameplayAbilitySpec* AbilitySpec = AbilitySystem->FindAbilitySpecFromClass(OrderDefinition.AbilityClass);
	if (!AbilitySpec)
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_MissingAbility,
			NSLOCTEXT("TCFOrders", "OrderAbilityNotGranted", "Source squad has not been granted the ability required by this order."));
	}

	FGameplayAbilityActorInfo* ActorInfo = AbilitySystem->AbilityActorInfo.Get();
	if (!ActorInfo)
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_InvalidSource,
			NSLOCTEXT("TCFOrders", "OrderExecutionMissingActorInfo", "Source squad Ability System Component has no actor info."));
	}

	UTCFOrderPayload* Payload = NewObject<UTCFOrderPayload>(AbilitySystem);

	// Order definitions are authored data assets. The payload stores a non-const UObject pointer for reflection/Blueprint access only.
	Payload->Initialize(Request, const_cast<UTCFOrderDefinition*>(&OrderDefinition));

	FGameplayEventData EventData;
	EventData.EventTag = OrderDefinition.OrderTag;
	EventData.Instigator = Request.SourceActor;
	EventData.Target = Request.Target.HasActorTarget() ? Request.Target.TargetActor : Request.SourceActor;
	EventData.OptionalObject = Payload;
	EventData.EventMagnitude = static_cast<float>(Request.OrderSequence);

	const bool bActivated = AbilitySystem->TriggerAbilityFromGameplayEvent(
		AbilitySpec->Handle,
		ActorInfo,
		OrderDefinition.OrderTag,
		&EventData,
		*AbilitySystem);

	if (!bActivated)
	{
		return FTCFOrderResult::Failure(
			TCFGameplayTags::Order_Result_Failed_ActivationFailed,
			NSLOCTEXT("TCFOrders", "OrderAbilityActivationFailed", "GAS failed to activate the ability for this order."));
	}

	return FTCFOrderResult::Success(TCFGameplayTags::Order_Result_Success);
}

bool UTCFOrderSubsystem::DoesSourceHaveRequiredTags(
	const FGameplayTagContainer& OwnedTags,
	const FGameplayTagContainer& RequiredTags,
	FGameplayTagContainer& OutMissingTags)
{
	OutMissingTags.Reset();

	for (const FGameplayTag& RequiredTag : RequiredTags)
	{
		if (!OwnedTags.HasTag(RequiredTag))
		{
			OutMissingTags.AddTag(RequiredTag);
		}
	}

	return OutMissingTags.IsEmpty();
}

bool UTCFOrderSubsystem::DoesSourceHaveBlockedTags(
	const FGameplayTagContainer& OwnedTags,
	const FGameplayTagContainer& BlockedTags,
	FGameplayTagContainer& OutBlockingTags)
{
	OutBlockingTags.Reset();

	for (const FGameplayTag& BlockedTag : BlockedTags)
	{
		if (OwnedTags.HasTag(BlockedTag))
		{
			OutBlockingTags.AddTag(BlockedTag);
		}
	}

	return !OutBlockingTags.IsEmpty();
}

bool UTCFOrderSubsystem::IsTargetTypeValid(const FTCFOrderTarget& Target, const UTCFOrderDefinition& OrderDefinition)
{
	switch (OrderDefinition.Targeting.TargetType)
	{
	case ETCFOrderTargetType::None:
		return Target.TargetType == ETCFOrderTargetType::None;

	case ETCFOrderTargetType::Self:
		return Target.TargetType == ETCFOrderTargetType::Self || Target.TargetType == ETCFOrderTargetType::None;

	case ETCFOrderTargetType::Actor:
		return Target.TargetType == ETCFOrderTargetType::Actor && Target.HasActorTarget();

	case ETCFOrderTargetType::Location:
		return Target.TargetType == ETCFOrderTargetType::Location;

	case ETCFOrderTargetType::Direction:
		return Target.HasDirectionTarget();

	case ETCFOrderTargetType::Area:
		return Target.TargetType == ETCFOrderTargetType::Area || Target.TargetType == ETCFOrderTargetType::Location;

	default:
		return false;
	}
}

bool UTCFOrderSubsystem::IsTargetWithinRange(
	const AActor& SourceActor,
	const FTCFOrderTarget& Target,
	const UTCFOrderDefinition& OrderDefinition)
{
	if (OrderDefinition.Targeting.Range <= 0.0f)
	{
		return true;
	}

	if (OrderDefinition.Targeting.TargetType == ETCFOrderTargetType::None
		|| OrderDefinition.Targeting.TargetType == ETCFOrderTargetType::Self)
	{
		return true;
	}

	FVector TargetLocation = FVector::ZeroVector;
	if (!TryGetTargetLocation(Target, TargetLocation))
	{
		return false;
	}

	const float RangeSquared = FMath::Square(OrderDefinition.Targeting.Range);
	return FVector::DistSquared(SourceActor.GetActorLocation(), TargetLocation) <= RangeSquared;
}

bool UTCFOrderSubsystem::TryGetTargetLocation(const FTCFOrderTarget& Target, FVector& OutLocation)
{
	if (Target.HasActorTarget())
	{
		OutLocation = Target.TargetActor->GetActorLocation();
		return true;
	}

	if (Target.TargetType == ETCFOrderTargetType::Location || Target.TargetType == ETCFOrderTargetType::Area)
	{
		OutLocation = Target.TargetLocation;
		return true;
	}

	return false;
}