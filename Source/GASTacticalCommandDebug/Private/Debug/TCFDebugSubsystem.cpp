//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Debug/TCFDebugSubsystem.h"

#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerOrderComponent.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "GAS/TCFSquadAttributeSet.h"
#include "GameplayEffect.h"
#include "TimerManager.h"
#include "Actors/TCFCapturePointActor.h"
#include "Components/TCFAffiliationComponent.h"
#include "Components/TCFCapturePointComponent.h"
#include "Subsystems/TCFRelationshipSubsystem.h"
#include "Subsystems/TCFSquadQuerySubsystem.h"
#include "Types/TCFAffiliationTypes.h"
#include "Types/TCFCaptureTypes.h"
#include "UI/TCFDebugHUDWidget.h"

void UTCFDebugSubsystem::RegisterPlayerController(APlayerController* PlayerController, float InRefreshInterval)
{
	if (!PlayerController)
	{
		return;
	}

	UnbindObservedComponents();

	ObservedPlayerController = PlayerController;
	RefreshInterval = FMath::Max(0.05f, InRefreshInterval);

	ObservedSelectionComponent = PlayerController->FindComponentByClass<UTCFPlayerSelectionComponent>();
	ObservedOrderComponent = PlayerController->FindComponentByClass<UTCFPlayerOrderComponent>();

	BindObservedComponents();
	RefreshDebugSnapshot();
	StartRefreshTimer();
}

void UTCFDebugSubsystem::UnregisterPlayerController(APlayerController* PlayerController)
{
	if (ObservedPlayerController.Get() != PlayerController)
	{
		return;
	}

	UnbindObservedComponents();
	StopRefreshTimer();

	ObservedPlayerController = nullptr;
	ObservedSelectionComponent = nullptr;
	ObservedOrderComponent = nullptr;
}

void UTCFDebugSubsystem::ShowDebugHUD()
{
	if (DebugWidget)
	{
		DebugWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		DebugWidget->SetDebugSnapshot(CurrentSnapshot);
		return;
	}

	APlayerController* PlayerController = ObservedPlayerController.Get();
	if (!PlayerController)
	{
		return;
	}

	DebugWidget = CreateWidget<UTCFDebugHUDWidget>(PlayerController, UTCFDebugHUDWidget::StaticClass());
	if (!DebugWidget)
	{
		return;
	}

	DebugWidget->AddToViewport(100);
	DebugWidget->SetDebugSnapshot(CurrentSnapshot);
}

void UTCFDebugSubsystem::HideDebugHUD()
{
	if (DebugWidget)
	{
		DebugWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTCFDebugSubsystem::ToggleDebugHUD()
{
	if (!DebugWidget || DebugWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		ShowDebugHUD();
		return;
	}

	HideDebugHUD();
}

void UTCFDebugSubsystem::RefreshDebugSnapshot()
{
	CurrentSnapshot = BuildSnapshot();

	if (DebugWidget)
	{
		DebugWidget->SetDebugSnapshot(CurrentSnapshot);
	}
}

const FTCFDebugSquadSnapshot& UTCFDebugSubsystem::GetCurrentSnapshot() const
{
	return CurrentSnapshot;
}

void UTCFDebugSubsystem::HandleSelectedSquadChanged(ATCFSquadActor* SelectedSquad)
{
	RefreshDebugSnapshot();
}

void UTCFDebugSubsystem::HandleOrderSubmitted(FTCFSquadOrderRequest Request, FTCFOrderResult Result)
{
	CurrentSnapshot.LastOrder = BuildOrderSnapshot(Request, Result);
	CurrentSnapshot = BuildSnapshot();
	CurrentSnapshot.LastOrder = BuildOrderSnapshot(Request, Result);

	if (DebugWidget)
	{
		DebugWidget->SetDebugSnapshot(CurrentSnapshot);
	}
}

void UTCFDebugSubsystem::BindObservedComponents()
{
	if (ObservedSelectionComponent)
	{
		ObservedSelectionComponent->OnSelectedSquadChanged.AddDynamic(this, &UTCFDebugSubsystem::HandleSelectedSquadChanged);
	}

	if (ObservedOrderComponent)
	{
		ObservedOrderComponent->OnOrderSubmitted.AddDynamic(this, &UTCFDebugSubsystem::HandleOrderSubmitted);
	}
}

void UTCFDebugSubsystem::UnbindObservedComponents()
{
	if (ObservedSelectionComponent)
	{
		ObservedSelectionComponent->OnSelectedSquadChanged.RemoveDynamic(this, &UTCFDebugSubsystem::HandleSelectedSquadChanged);
	}

	if (ObservedOrderComponent)
	{
		ObservedOrderComponent->OnOrderSubmitted.RemoveDynamic(this, &UTCFDebugSubsystem::HandleOrderSubmitted);
	}
}

void UTCFDebugSubsystem::StartRefreshTimer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().SetTimer(
		RefreshTimerHandle,
		this,
		&UTCFDebugSubsystem::RefreshDebugSnapshot,
		RefreshInterval,
		true);
}

void UTCFDebugSubsystem::StopRefreshTimer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(RefreshTimerHandle);
}

FTCFDebugSquadSnapshot UTCFDebugSubsystem::BuildSnapshot() const
{
	FTCFDebugSquadSnapshot Snapshot;
	Snapshot.LastOrder = CurrentSnapshot.LastOrder;

	const UTCFPlayerSelectionComponent* SelectionComponent = ObservedSelectionComponent;
	if (!SelectionComponent)
	{
		return Snapshot;
	}

	const ATCFSquadActor* SelectedSquad = SelectionComponent->GetSelectedSquad();
	if (!IsValid(SelectedSquad))
	{
		return Snapshot;
	}

	Snapshot.bHasSelectedSquad = true;
	Snapshot.ActorName = SelectedSquad->GetName();
	Snapshot.DisplayName = SelectedSquad->GetDisplayName();
	Snapshot.bInitialized = SelectedSquad->IsInitialized();
	Snapshot.RoleTag = SelectedSquad->GetRoleTag();
	
	AddAffiliationData(*SelectedSquad, Snapshot);
	AddRelationshipLines(*SelectedSquad, Snapshot);
	AddNearestCapturePointData(*SelectedSquad, Snapshot);

	if (const UAbilitySystemComponent* AbilitySystem = SelectedSquad->GetAbilitySystemComponent())
	{
		FGameplayTagContainer OwnedTags;
		AbilitySystem->GetOwnedGameplayTags(OwnedTags);
		Snapshot.OwnedTags = OwnedTags.ToStringSimple();

		AddAbilityLines(*AbilitySystem, Snapshot);
		AddActiveEffectLines(*AbilitySystem, Snapshot);
	}

	AddAttributeLines(*SelectedSquad, Snapshot);

	return Snapshot;
}

FTCFDebugOrderSnapshot UTCFDebugSubsystem::BuildOrderSnapshot(
	const FTCFSquadOrderRequest& Request,
	const FTCFOrderResult& Result) const
{
	FTCFDebugOrderSnapshot Snapshot;
	Snapshot.bHasOrder = true;
	Snapshot.OrderSequence = Request.OrderSequence;
	Snapshot.OrderTag = Request.OrderTag;
	Snapshot.SourceName = GetNameSafe(Request.SourceActor);
	Snapshot.TargetSummary = BuildTargetSummary(Request.Target);
	Snapshot.bSuccess = Result.bSuccess;
	Snapshot.ResultTag = Result.ResultTag;
	Snapshot.BlockingTags = Result.BlockingTags;
	Snapshot.FailureReason = Result.FailureReason;

	return Snapshot;
}

FString UTCFDebugSubsystem::BuildTargetSummary(const FTCFOrderTarget& Target)
{
	switch (Target.TargetType)
	{
	case ETCFOrderTargetType::None:
		return TEXT("None");

	case ETCFOrderTargetType::Self:
		return TEXT("Self");

	case ETCFOrderTargetType::Actor:
		return FString::Printf(TEXT("Actor: %s"), *GetNameSafe(Target.TargetActor));

	case ETCFOrderTargetType::Location:
		return FString::Printf(TEXT("Location: %s"), *Target.TargetLocation.ToCompactString());

	case ETCFOrderTargetType::Direction:
		return FString::Printf(TEXT("Direction: %s"), *Target.TargetDirection.ToCompactString());

	case ETCFOrderTargetType::Area:
		return FString::Printf(
			TEXT("Area: %s | Radius %.1f | Cone %.1f"),
			*Target.TargetLocation.ToCompactString(),
			Target.Radius,
			Target.ConeAngleDegrees);

	default:
		return TEXT("Unknown");
	}
}

void UTCFDebugSubsystem::AddAttributeLines(const ATCFSquadActor& Squad, FTCFDebugSquadSnapshot& Snapshot)
{
	const UTCFSquadAttributeSet* AttributeSet = Squad.GetSquadAttributeSet();
	if (!AttributeSet)
	{
		Snapshot.AttributeLines.Add(TEXT("No AttributeSet"));
		return;
	}

	Snapshot.AttributeLines.Add(FString::Printf(TEXT("Health: %.1f / %.1f"),AttributeSet->GetHealth(), AttributeSet->GetMaxHealth()));
	Snapshot.AttributeLines.Add(FString::Printf(TEXT("Morale: %.1f"), AttributeSet->GetMorale()));
	Snapshot.AttributeLines.Add(FString::Printf(TEXT("Suppression: %.1f"), AttributeSet->GetSuppression()));
	Snapshot.AttributeLines.Add(FString::Printf(TEXT("Cohesion: %.1f"), AttributeSet->GetCohesion()));
	Snapshot.AttributeLines.Add(FString::Printf(TEXT("Stamina: %.1f"), AttributeSet->GetStamina()));
	Snapshot.AttributeLines.Add(FString::Printf(TEXT("Accuracy: %.2f"), AttributeSet->GetAccuracy()));
	Snapshot.AttributeLines.Add(FString::Printf(TEXT("Defense: %.2f"), AttributeSet->GetDefense()));
	Snapshot.AttributeLines.Add(FString::Printf(TEXT("Move Speed: %.1f"), AttributeSet->GetMovementSpeed()));
	Snapshot.AttributeLines.Add(FString::Printf(TEXT("Capture Power: %.2f"), AttributeSet->GetCapturePower()));
}

void UTCFDebugSubsystem::AddAbilityLines(const UAbilitySystemComponent& AbilitySystem, FTCFDebugSquadSnapshot& Snapshot)
{
	for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystem.GetActivatableAbilities())
	{
		const UGameplayAbility* Ability = AbilitySpec.Ability;
		const FString AbilityName = Ability && Ability->GetClass()
			? Ability->GetClass()->GetName()
			: TEXT("Unknown Ability");

		const FString SourceTags = AbilitySpec.GetDynamicSpecSourceTags().ToStringSimple();

		Snapshot.AbilityLines.Add(FString::Printf(
			TEXT("%s | Tags: %s"),
			*AbilityName,
			SourceTags.IsEmpty() ? TEXT("None") : *SourceTags));
	}
}

void UTCFDebugSubsystem::AddActiveEffectLines(const UAbilitySystemComponent& AbilitySystem, FTCFDebugSquadSnapshot& Snapshot) const
{
	const UWorld* World = GetWorld();
	const float WorldTime = World ? World->GetTimeSeconds() : 0.0f;

	const FGameplayEffectQuery Query;
	const TArray<FActiveGameplayEffectHandle> ActiveEffectHandles = AbilitySystem.GetActiveEffects(Query);

	for (const FActiveGameplayEffectHandle& Handle : ActiveEffectHandles)
	{
		const FActiveGameplayEffect* ActiveEffect = AbilitySystem.GetActiveGameplayEffect(Handle);
		if (!ActiveEffect || !ActiveEffect->Spec.Def)
		{
			continue;
		}

		const FString EffectName = ActiveEffect->Spec.Def->GetName();
		const float Duration = ActiveEffect->GetDuration();
		const float TimeRemaining = ActiveEffect->GetTimeRemaining(WorldTime);

		if (Duration > 0.0f)
		{
			Snapshot.ActiveEffectLines.Add(FString::Printf(
				TEXT("%s | %.1fs / %.1fs"),
				*EffectName,
				FMath::Max(0.0f, TimeRemaining),
				Duration));
		}
		else
		{
			Snapshot.ActiveEffectLines.Add(FString::Printf(
				TEXT("%s | Infinite/Instant"),
				*EffectName));
		}
	}
}

FString UTCFDebugSubsystem::RelationshipToString(ETCFSquadRelationship Relationship)
{
	switch (Relationship)
	{
	case ETCFSquadRelationship::Own:
		return TEXT("Own");

	case ETCFSquadRelationship::Friendly:
		return TEXT("Friendly");

	case ETCFSquadRelationship::Neutral:
		return TEXT("Neutral");

	case ETCFSquadRelationship::Enemy:
		return TEXT("Enemy");

	default:
		return TEXT("Unknown");
	}
}

FString UTCFDebugSubsystem::CaptureStateToString(ETCFCapturePointState State)
{
	switch (State)
	{
	case ETCFCapturePointState::Neutral:
		return TEXT("Neutral");

	case ETCFCapturePointState::Owned:
		return TEXT("Owned");

	case ETCFCapturePointState::Capturing:
		return TEXT("Capturing");

	case ETCFCapturePointState::Contested:
		return TEXT("Contested");

	default:
		return TEXT("Unknown");
	}
}

void UTCFDebugSubsystem::AddAffiliationData(const ATCFSquadActor& Squad, FTCFDebugSquadSnapshot& Snapshot)
{
	const UTCFAffiliationComponent* AffiliationComponent = Squad.GetAffiliationComponent();
	if (!AffiliationComponent)
	{
		Snapshot.bHasAffiliation = false;
		return;
	}

	const FTCFAffiliationData& Affiliation = AffiliationComponent->GetAffiliation();

	Snapshot.bHasAffiliation = true;
	Snapshot.OwnerId = Affiliation.OwnerId;
	Snapshot.TeamId = Affiliation.TeamId;
	Snapshot.FactionTag = Affiliation.FactionTag;
}

void UTCFDebugSubsystem::AddRelationshipLines(const ATCFSquadActor& Squad, FTCFDebugSquadSnapshot& Snapshot) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UTCFSquadQuerySubsystem* SquadQuerySubsystem = World->GetSubsystem<UTCFSquadQuerySubsystem>();
	const UTCFRelationshipSubsystem* RelationshipSubsystem = World->GetSubsystem<UTCFRelationshipSubsystem>();
	if (!SquadQuerySubsystem || !RelationshipSubsystem)
	{
		return;
	}

	TArray<ATCFSquadActor*> NearbySquads;
	SquadQuerySubsystem->GetSquadsInRadius(
		Squad.GetActorLocation(),
		2500.0f,
		&Squad,
		NearbySquads);

	if (NearbySquads.IsEmpty())
	{
		Snapshot.RelationshipLines.Add(TEXT("No nearby registered squads."));
		return;
	}

	for (const ATCFSquadActor* OtherSquad : NearbySquads)
	{
		if (!IsValid(OtherSquad))
		{
			continue;
		}

		const ETCFSquadRelationship Relationship = RelationshipSubsystem->GetActorRelationship(&Squad, OtherSquad);

		Snapshot.RelationshipLines.Add(FString::Printf(
			TEXT("%s: %s"),
			*OtherSquad->GetName(),
			*RelationshipToString(Relationship)));
	}
}

void UTCFDebugSubsystem::AddNearestCapturePointData(const ATCFSquadActor& Squad, FTCFDebugSquadSnapshot& Snapshot) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ATCFCapturePointActor* NearestPoint = nullptr;
	float BestDistanceSquared = TNumericLimits<float>::Max();

	const FVector SquadLocation = Squad.GetActorLocation();

	for (TActorIterator<ATCFCapturePointActor> It(World); It; ++It)
	{
		ATCFCapturePointActor* CapturePoint = *It;
		if (!IsValid(CapturePoint))
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(SquadLocation, CapturePoint->GetActorLocation());
		if (DistanceSquared < BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			NearestPoint = CapturePoint;
		}
	}

	if (!IsValid(NearestPoint))
	{
		Snapshot.NearestCapturePoint.bHasCapturePoint = false;
		return;
	}

	const UTCFCapturePointComponent* CaptureComponent = NearestPoint->GetCapturePointComponent();
	if (!CaptureComponent)
	{
		Snapshot.NearestCapturePoint.bHasCapturePoint = false;
		return;
	}

	Snapshot.NearestCapturePoint.bHasCapturePoint = true;
	Snapshot.NearestCapturePoint.ActorName = NearestPoint->GetName();
	Snapshot.NearestCapturePoint.State = CaptureStateToString(CaptureComponent->GetCaptureState());
	Snapshot.NearestCapturePoint.OwnerSide = CaptureComponent->GetOwnerSide().ToDebugString();
	Snapshot.NearestCapturePoint.PendingSide = CaptureComponent->GetPendingCaptureSide().ToDebugString();
	Snapshot.NearestCapturePoint.CaptureProgress = CaptureComponent->GetCaptureProgress();
	Snapshot.NearestCapturePoint.CaptureThreshold = CaptureComponent->GetCaptureThreshold();
	Snapshot.NearestCapturePoint.CaptureRadius = CaptureComponent->GetCaptureRadius();
	Snapshot.NearestCapturePoint.OccupyingSquadCount = CaptureComponent->GetLastOccupyingSquadCount();
	Snapshot.NearestCapturePoint.DistanceFromSelectedSquad = FMath::Sqrt(BestDistanceSquared);
}
