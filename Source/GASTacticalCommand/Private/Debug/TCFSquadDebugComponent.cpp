//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Debug/TCFSquadDebugComponent.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerOrderComponent.h"
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
	if (ObservedSelectionComponent)
	{
		ObservedSelectionComponent->OnSelectedSquadChanged.AddDynamic(this, &UTCFSquadDebugComponent::HandleSelectedSquadChanged);
	}
	else
	{
		UE_LOG(LogTCF, Warning, TEXT("TCF Debug: '%s' could not find a PlayerSelectionComponent to observe."), *GetName());
	}

	ObservedOrderComponent = FindOrderComponent();
	if (ObservedOrderComponent)
	{
		ObservedOrderComponent->OnOrderSubmitted.AddDynamic(this, &UTCFSquadDebugComponent::HandleOrderSubmitted);
	}
	else
	{
		UE_LOG(LogTCF, Warning, TEXT("TCF Debug: '%s' could not find a PlayerOrderComponent to observe."), *GetName());
	}
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

void UTCFSquadDebugComponent::LogOrderSubmission(const FTCFSquadOrderRequest& Request, const FTCFOrderResult& Result) const
{
	const AActor* SourceActor = Request.SourceActor;
	const ATCFSquadActor* SourceSquad = Cast<ATCFSquadActor>(SourceActor);

	const FString SourceName = SourceSquad ? SourceSquad->GetName() : GetNameSafe(SourceActor);
	const FString TargetString = BuildOrderTargetDebugString(Request.Target);
	const FString ResultString = BuildOrderResultDebugString(Result);

	UE_LOG(LogTCF, Log,
		TEXT("TCF Debug Order: Seq=%d | Order=%s | Source=%s | Target=%s | %s"),
		Request.OrderSequence,
		*Request.OrderTag.ToString(),
		*SourceName,
		*TargetString,
		*ResultString);
}

void UTCFSquadDebugComponent::HandleSelectedSquadChanged(ATCFSquadActor* SelectedSquad)
{
	if (!bLogSelectionChanges)
	{
		return;
	}

	LogSquadState(SelectedSquad);
}

void UTCFSquadDebugComponent::HandleOrderSubmitted(FTCFSquadOrderRequest Request, FTCFOrderResult Result)
{
	if (!bLogOrderSubmissions)
	{
		return;
	}

	LogOrderSubmission(Request, Result);
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

UTCFPlayerOrderComponent* UTCFSquadDebugComponent::FindOrderComponent() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	return Owner->FindComponentByClass<UTCFPlayerOrderComponent>();
}

FString UTCFSquadDebugComponent::BuildOrderTargetDebugString(const FTCFOrderTarget& Target)
{
	switch (Target.TargetType)
	{
	case ETCFOrderTargetType::None:
		return TEXT("None");

	case ETCFOrderTargetType::Self:
		return TEXT("Self");

	case ETCFOrderTargetType::Actor:
		return FString::Printf(
			TEXT("Actor=%s"),
			*GetNameSafe(Target.TargetActor));

	case ETCFOrderTargetType::Location:
		return FString::Printf(
			TEXT("Location=%s"),
			*Target.TargetLocation.ToCompactString());

	case ETCFOrderTargetType::Direction:
		return FString::Printf(
			TEXT("Direction=%s"),
			*Target.TargetDirection.ToCompactString());

	case ETCFOrderTargetType::Area:
		return FString::Printf(
			TEXT("Area Location=%s Radius=%.2f Cone=%.2f"),
			*Target.TargetLocation.ToCompactString(),
			Target.Radius,
			Target.ConeAngleDegrees);

	default:
		return TEXT("Unknown");
	}
}

FString UTCFSquadDebugComponent::BuildOrderResultDebugString(const FTCFOrderResult& Result)
{
	const FString BlockingTagsString = Result.BlockingTags.IsEmpty()
		? TEXT("None")
		: Result.BlockingTags.ToStringSimple();

	const FString FailureReasonString = Result.FailureReason.IsEmpty()
		? TEXT("None")
		: Result.FailureReason.ToString();

	return FString::Printf(
		TEXT("Success=%s | Result=%s | BlockingTags=%s | Reason=%s"),
		Result.bSuccess ? TEXT("true") : TEXT("false"),
		*Result.ResultTag.ToString(),
		*BlockingTagsString,
		*FailureReasonString);
}