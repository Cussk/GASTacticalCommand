//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFCapturePointComponent.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFAffiliationComponent.h"
#include "GAS/TCFSquadAttributeSet.h"
#include "Subsystems/TCFSquadQuerySubsystem.h"
#include "TCFGameplayTags.h"
#include "TimerManager.h"

UTCFCapturePointComponent::UTCFCapturePointComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFCapturePointComponent::BeginPlay()
{
	Super::BeginPlay();

	AffiliationComponent = GetOwner()
		? GetOwner()->FindComponentByClass<UTCFAffiliationComponent>()
		: nullptr;

	RefreshOwnerSideFromAffiliation();

	CaptureState = OwnerSide.IsValid()
		? ETCFCapturePointState::Owned
		: ETCFCapturePointState::Neutral;

	if (UWorld* World = GetWorld())
	{
		LastUpdateTimeSeconds = World->GetTimeSeconds();

		World->GetTimerManager().SetTimer(
			CaptureUpdateTimerHandle,
			this,
			&UTCFCapturePointComponent::UpdateCapture,
			UpdateInterval,
			true);
	}
}

void UTCFCapturePointComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CaptureUpdateTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

ETCFCapturePointState UTCFCapturePointComponent::GetCaptureState() const
{
	return CaptureState;
}

float UTCFCapturePointComponent::GetCaptureProgress() const
{
	return CaptureProgress;
}

float UTCFCapturePointComponent::GetCaptureThreshold() const
{
	return CaptureThreshold;
}

float UTCFCapturePointComponent::GetCaptureRadius() const
{
	return CaptureRadius;
}

FTCFCaptureSideKey UTCFCapturePointComponent::GetOwnerSide() const
{
	return OwnerSide;
}

FTCFCaptureSideKey UTCFCapturePointComponent::GetPendingCaptureSide() const
{
	return PendingCaptureSide;
}

int32 UTCFCapturePointComponent::GetLastOccupyingSquadCount() const
{
	return LastOccupyingSquadCount;
}

void UTCFCapturePointComponent::ForceCaptureRefresh()
{
	UpdateCapture();
}

void UTCFCapturePointComponent::UpdateCapture()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float CurrentTimeSeconds = World->GetTimeSeconds();
	const float DeltaTime = FMath::Max(0.0f, CurrentTimeSeconds - LastUpdateTimeSeconds);
	LastUpdateTimeSeconds = CurrentTimeSeconds;

	EvaluateCapture(DeltaTime);
}

void UTCFCapturePointComponent::EvaluateCapture(float DeltaTime)
{
	TArray<FTCFCaptureSideScore> Scores;
	GatherCaptureScores(Scores);

	LastOccupyingSquadCount = 0;
	for (const FTCFCaptureSideScore& Score : Scores)
	{
		LastOccupyingSquadCount += Score.SquadCount;
	}

	if (Scores.IsEmpty())
	{
		if (bDecayProgressWhenEmpty)
		{
			DecayCaptureProgress(DeltaTime);
		}

		SetCaptureState(OwnerSide.IsValid()
			? ETCFCapturePointState::Owned
			: ETCFCapturePointState::Neutral);

		return;
	}

	Scores.Sort([](const FTCFCaptureSideScore& A, const FTCFCaptureSideScore& B)
	{
		return A.TotalCapturePower > B.TotalCapturePower;
	});

	const FTCFCaptureSideScore& TopScore = Scores[0];
	const float SecondBestPower = Scores.Num() > 1 ? Scores[1].TotalCapturePower : 0.0f;

	if (Scores.Num() > 1 && FMath::IsNearlyEqual(TopScore.TotalCapturePower, SecondBestPower, KINDA_SMALL_NUMBER))
	{
		SetCaptureState(ETCFCapturePointState::Contested);
		return;
	}

	if (OwnerSide.IsValid() && TopScore.SideKey == OwnerSide)
	{
		DecayCaptureProgress(DeltaTime);
		SetCaptureState(ETCFCapturePointState::Owned);
		return;
	}

	ApplyTopSideCapture(TopScore, SecondBestPower, DeltaTime);
}

void UTCFCapturePointComponent::GatherCaptureScores(TArray<FTCFCaptureSideScore>& OutScores) const
{
	OutScores.Reset();

	UWorld* World = GetWorld();
	if (!World || !GetOwner())
	{
		return;
	}

	UTCFSquadQuerySubsystem* SquadQuerySubsystem = World->GetSubsystem<UTCFSquadQuerySubsystem>();
	if (!SquadQuerySubsystem)
	{
		return;
	}

	TArray<ATCFSquadActor*> SquadsInRadius;
	SquadQuerySubsystem->GetSquadsInRadius(
		GetOwner()->GetActorLocation(),
		CaptureRadius,
		nullptr,
		SquadsInRadius);

	TMap<FTCFCaptureSideKey, FTCFCaptureSideScore> ScoreMap;

	for (ATCFSquadActor* Squad : SquadsInRadius)
	{
		if (!IsValid(Squad) || !ShouldIncludeSquad(*Squad))
		{
			continue;
		}

		const UTCFAffiliationComponent* SquadAffiliationComponent = Squad->GetAffiliationComponent();
		if (!SquadAffiliationComponent)
		{
			continue;
		}

		const FTCFAffiliationData& SquadAffiliation = SquadAffiliationComponent->GetAffiliation();
		const FTCFCaptureSideKey SideKey = FTCFCaptureSideKey::FromAffiliation(SquadAffiliation);
		if (!SideKey.IsValid())
		{
			continue;
		}

		const float CapturePower = GetSquadCapturePower(*Squad);
		if (CapturePower <= 0.0f)
		{
			continue;
		}

		FTCFCaptureSideScore& Score = ScoreMap.FindOrAdd(SideKey);
		if (!Score.SideKey.IsValid())
		{
			Score.SideKey = SideKey;
			Score.RepresentativeAffiliation = SquadAffiliation;
		}

		Score.TotalCapturePower += CapturePower;
		Score.SquadCount++;
	}

	ScoreMap.GenerateValueArray(OutScores);
}

bool UTCFCapturePointComponent::ShouldIncludeSquad(const ATCFSquadActor& Squad) const
{
	if (!bRequireStationarySquads)
	{
		return true;
	}

	const UAbilitySystemComponent* AbilitySystem = Squad.GetAbilitySystemComponent();
	if (!AbilitySystem)
	{
		return false;
	}

	return AbilitySystem->HasMatchingGameplayTag(TCFGameplayTags::Squad_State_Stationary);
}

float UTCFCapturePointComponent::GetSquadCapturePower(const ATCFSquadActor& Squad) const
{
	const UTCFSquadAttributeSet* AttributeSet = Squad.GetSquadAttributeSet();
	if (!AttributeSet)
	{
		return 0.0f;
	}

	return FMath::Max(0.0f, AttributeSet->GetCapturePower());
}

void UTCFCapturePointComponent::ApplyTopSideCapture(
	const FTCFCaptureSideScore& TopScore,
	float SecondBestPower,
	float DeltaTime)
{
	const float NetCapturePower = FMath::Max(0.0f, TopScore.TotalCapturePower - SecondBestPower);
	if (NetCapturePower <= 0.0f)
	{
		SetCaptureState(ETCFCapturePointState::Contested);
		return;
	}

	const float ProgressDelta = NetCapturePower * CaptureProgressPerPowerPerSecond * DeltaTime;

	if (PendingCaptureSide.IsValid()
		&& PendingCaptureSide != TopScore.SideKey
		&& CaptureProgress > 0.0f)
	{
		SetCaptureProgress(CaptureProgress - ProgressDelta);

		if (CaptureProgress <= 0.0f)
		{
			PendingCaptureSide = TopScore.SideKey;
			PendingCaptureAffiliation = TopScore.RepresentativeAffiliation;
		}

		SetCaptureState(ETCFCapturePointState::Contested);
		return;
	}

	if (!PendingCaptureSide.IsValid() || PendingCaptureSide != TopScore.SideKey)
	{
		PendingCaptureSide = TopScore.SideKey;
		PendingCaptureAffiliation = TopScore.RepresentativeAffiliation;
		SetCaptureProgress(0.0f);
	}

	SetCaptureProgress(CaptureProgress + ProgressDelta);
	SetCaptureState(ETCFCapturePointState::Capturing);

	if (CaptureProgress >= CaptureThreshold)
	{
		CompleteCapture(TopScore);
	}
}

void UTCFCapturePointComponent::DecayCaptureProgress(float DeltaTime)
{
	if (CaptureProgress <= 0.0f)
	{
		PendingCaptureSide = FTCFCaptureSideKey();
		PendingCaptureAffiliation = FTCFAffiliationData();
		SetCaptureProgress(0.0f);
		return;
	}

	SetCaptureProgress(CaptureProgress - EmptyProgressDecayPerSecond * DeltaTime);

	if (CaptureProgress <= 0.0f)
	{
		PendingCaptureSide = FTCFCaptureSideKey();
		PendingCaptureAffiliation = FTCFAffiliationData();
		SetCaptureProgress(0.0f);
	}
}

void UTCFCapturePointComponent::CompleteCapture(const FTCFCaptureSideScore& CapturingSide)
{
	UTCFAffiliationComponent* OwnerAffiliationComponent = GetAffiliationComponent();
	if (!OwnerAffiliationComponent)
	{
		return;
	}

	OwnerAffiliationComponent->SetAffiliation(CapturingSide.RepresentativeAffiliation);

	OwnerSide = CapturingSide.SideKey;
	PendingCaptureSide = FTCFCaptureSideKey();
	PendingCaptureAffiliation = FTCFAffiliationData();

	SetCaptureProgress(0.0f);
	SetCaptureState(ETCFCapturePointState::Owned);

	OnCaptureOwnerChanged.Broadcast(
		OwnerAffiliationComponent->GetAffiliationForBlueprint(),
		OwnerSide);
}

void UTCFCapturePointComponent::RefreshOwnerSideFromAffiliation()
{
	const UTCFAffiliationComponent* OwnerAffiliationComponent = GetAffiliationComponent();
	if (!OwnerAffiliationComponent)
	{
		OwnerSide = FTCFCaptureSideKey();
		return;
	}

	OwnerSide = FTCFCaptureSideKey::FromAffiliation(OwnerAffiliationComponent->GetAffiliation());
}

void UTCFCapturePointComponent::SetCaptureState(ETCFCapturePointState NewState)
{
	if (CaptureState == NewState)
	{
		return;
	}

	CaptureState = NewState;
	OnCaptureStateChanged.Broadcast(CaptureState);
}

void UTCFCapturePointComponent::SetCaptureProgress(float NewProgress)
{
	const float ClampedProgress = FMath::Clamp(NewProgress, 0.0f, CaptureThreshold);
	if (FMath::IsNearlyEqual(CaptureProgress, ClampedProgress, KINDA_SMALL_NUMBER))
	{
		return;
	}

	CaptureProgress = ClampedProgress;
	OnCaptureProgressChanged.Broadcast(CaptureProgress, CaptureThreshold);
}

UTCFAffiliationComponent* UTCFCapturePointComponent::GetAffiliationComponent() const
{
	if (AffiliationComponent)
	{
		return AffiliationComponent;
	}

	return GetOwner()
		? GetOwner()->FindComponentByClass<UTCFAffiliationComponent>()
		: nullptr;
}
