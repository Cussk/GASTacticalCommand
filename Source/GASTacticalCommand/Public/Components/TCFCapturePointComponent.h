//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/TCFCaptureTypes.h"
#include "TCFCapturePointComponent.generated.h"

class ATCFSquadActor;
class UTCFAffiliationComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFCapturePointStateChanged, ETCFCapturePointState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTCFCaptureProgressChanged, float, CaptureProgress, float, CaptureThreshold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTCFCaptureOwnerChanged, FTCFAffiliationData, NewOwnerAffiliation, FTCFCaptureSideKey, NewOwnerSide);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFCapturePointComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFCapturePointComponent();

	UFUNCTION(BlueprintPure, Category = "TCF|Capture")
	ETCFCapturePointState GetCaptureState() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Capture")
	float GetCaptureProgress() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Capture")
	float GetCaptureThreshold() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Capture")
	float GetCaptureRadius() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Capture")
	FTCFCaptureSideKey GetOwnerSide() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Capture")
	FTCFCaptureSideKey GetPendingCaptureSide() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Capture")
	int32 GetLastOccupyingSquadCount() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Capture")
	void ForceCaptureRefresh();

	UPROPERTY(BlueprintAssignable, Category = "TCF|Capture")
	FOnTCFCapturePointStateChanged OnCaptureStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Capture")
	FOnTCFCaptureProgressChanged OnCaptureProgressChanged;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Capture")
	FOnTCFCaptureOwnerChanged OnCaptureOwnerChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Capture", meta = (ClampMin = "1.0"))
	float CaptureRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Capture", meta = (ClampMin = "1.0"))
	float CaptureThreshold = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Capture", meta = (ClampMin = "0.01"))
	float CaptureProgressPerPowerPerSecond = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Capture", meta = (ClampMin = "0.01"))
	float UpdateInterval = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Capture")
	bool bRequireStationarySquads = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Capture")
	bool bDecayProgressWhenEmpty = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Capture", meta = (ClampMin = "0.0"))
	float EmptyProgressDecayPerSecond = 20.0f;

private:
	UPROPERTY()
	TObjectPtr<UTCFAffiliationComponent> AffiliationComponent;

	FTimerHandle CaptureUpdateTimerHandle;

	ETCFCapturePointState CaptureState = ETCFCapturePointState::Neutral;

	FTCFCaptureSideKey OwnerSide;
	FTCFCaptureSideKey PendingCaptureSide;
	FTCFAffiliationData PendingCaptureAffiliation;

	float CaptureProgress = 0.0f;
	float LastUpdateTimeSeconds = 0.0f;

	int32 LastOccupyingSquadCount = 0;

	void UpdateCapture();
	void EvaluateCapture(float DeltaTime);

	void GatherCaptureScores(TArray<FTCFCaptureSideScore>& OutScores) const;
	bool ShouldIncludeSquad(const ATCFSquadActor& Squad) const;
	float GetSquadCapturePower(const ATCFSquadActor& Squad) const;

	void ApplyTopSideCapture(const FTCFCaptureSideScore& TopScore, float SecondBestPower, float DeltaTime);
	void DecayCaptureProgress(float DeltaTime);
	void CompleteCapture(const FTCFCaptureSideScore& CapturingSide);

	void RefreshOwnerSideFromAffiliation();
	void SetCaptureState(ETCFCapturePointState NewState);
	void SetCaptureProgress(float NewProgress);

	UTCFAffiliationComponent* GetAffiliationComponent() const;
};