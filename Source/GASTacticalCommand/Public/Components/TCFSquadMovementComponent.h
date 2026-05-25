//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "TCFSquadMovementComponent.generated.h"

class ATCFSquadActor;
class UAbilitySystemComponent;
class UGameplayEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFSquadMovementChanged, bool, bIsMoving);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFSquadMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFSquadMovementComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Movement")
	bool MoveToLocation(FVector WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "TCF|Movement")
	void StopMovement(bool bSnapToTarget = false);

	UFUNCTION(BlueprintPure, Category = "TCF|Movement")
	bool IsMoving() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Movement")
	FVector GetMoveTargetLocation() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Movement")
	FOnTCFSquadMovementChanged OnMovementChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Movement", meta = (ClampMin = "1.0"))
	float AcceptanceRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Movement", meta = (ClampMin = "1.0"))
	float FallbackMoveSpeed = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Movement")
	bool bKeepCurrentZ = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|GAS State")
	TSubclassOf<UGameplayEffect> MovingStateEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|GAS State")
	TSubclassOf<UGameplayEffect> StationaryStateEffect;

private:
	UPROPERTY()
	TObjectPtr<ATCFSquadActor> SquadOwner;

	FVector TargetLocation = FVector::ZeroVector;

	bool bMoving = false;
	bool bUsingLooseMovingTag = false;
	bool bUsingLooseStationaryTag = false;

	FActiveGameplayEffectHandle MovingStateEffectHandle;
	FActiveGameplayEffectHandle StationaryStateEffectHandle;

	void StartMovingState();
	void StartStationaryState();
	void ClearMovingState();
	void ClearStationaryState();
	void FinishMovement();

	float GetCurrentMoveSpeed() const;

	UAbilitySystemComponent* GetAbilitySystem() const;
	FActiveGameplayEffectHandle ApplyStateEffect(TSubclassOf<UGameplayEffect> EffectClass) const;
};