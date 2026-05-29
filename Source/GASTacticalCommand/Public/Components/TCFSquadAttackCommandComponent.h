//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TCFSquadAttackCommandComponent.generated.h"

class ATCFSquadActor;
class UTCFOrderDefinition;
class UTCFRelationshipSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFSquadAttackCommandChanged,
	bool,
	bHasAttackCommand,
	AActor*,
	TargetActor);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFSquadAttackCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFSquadAttackCommandComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Attack Command")
	bool StartAttackCommand(AActor* NewTargetActor);

	UFUNCTION(BlueprintCallable, Category = "TCF|Attack Command")
	void StopAttackCommand();

	UFUNCTION(BlueprintPure, Category = "TCF|Attack Command")
	bool HasAttackCommand() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Attack Command")
	AActor* GetAttackTarget() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Attack Command")
	FOnTCFSquadAttackCommandChanged OnAttackCommandChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attack Command")
	FGameplayTag BasicAttackOrderTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attack Command", meta = (ClampMin = "0.05"))
	float CommandUpdateInterval = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attack Command", meta = (ClampMin = "0.0"))
	float FallbackAttackRange = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attack Command", meta = (ClampMin = "0.0"))
	float AttackRangePadding = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Attack Command", meta = (ClampMin = "0.0"))
	float MoveTargetRefreshDistance = 120.0f;

private:
	UPROPERTY()
	TObjectPtr<ATCFSquadActor> SquadOwner;

	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

	FTimerHandle AttackCommandTimerHandle;
	FVector LastMoveTargetLocation = FVector::ZeroVector;
	bool bHasIssuedMove = false;

	void TickAttackCommand();

	bool IsAttackCommandStillValid() const;
	bool IsTargetEnemy() const;
	bool IsTargetInAttackRange() const;

	float GetAttackRange() const;
	FVector GetDesiredAttackMoveLocation() const;

	bool SubmitBasicAttackOrder() const;
	void MoveTowardTarget();
	void StopMovement() const;

	void BindTargetDestroyed(AActor* NewTargetActor);
	void UnbindTargetDestroyed();

	UFUNCTION()
	void HandleTargetDestroyed(AActor* DestroyedActor);
};