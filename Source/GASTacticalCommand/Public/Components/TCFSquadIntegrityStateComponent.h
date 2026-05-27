//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCFSquadIntegrityStateComponent.generated.h"

struct FOnAttributeChangeData;
class ATCFSquadActor;
class UAbilitySystemComponent;
class UTCFSquadAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTCFSquadDefeated);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFSquadIntegrityStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFSquadIntegrityStateComponent();

	UFUNCTION(BlueprintPure, Category = "TCF|Squad Integrity")
	bool IsDefeated() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Squad Integrity")
	void ForceDefeat();

	UPROPERTY(BlueprintAssignable, Category = "TCF|Squad Integrity")
	FOnTCFSquadDefeated OnSquadDefeated;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Squad Integrity")
	bool bDestroyActorOnDefeat = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Squad Integrity", meta = (ClampMin = "0.0"))
	float DestroyDelaySeconds = 0.05f;

private:
	UPROPERTY()
	TObjectPtr<ATCFSquadActor> SquadOwner;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY()
	TObjectPtr<const UTCFSquadAttributeSet> AttributeSet;

	bool bDefeated = false;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle CohesionChangedDelegateHandle;
	FDelegateHandle MoraleChangedDelegateHandle;

	void BindAttributeDelegates();
	void UnbindAttributeDelegates();

	void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	void HandleCohesionChanged(const FOnAttributeChangeData& ChangeData);
	void HandleMoraleChanged(const FOnAttributeChangeData& ChangeData);

	void EvaluateInitialIntegrityState();
	void EnterDefeatedState();

	void StopMovement() const;
	void CancelActiveAbilities() const;
	void ApplyDeadTag() const;
	void ScheduleDestroy() const;
};