//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TCFSquadGatherCommandComponent.generated.h"

class ATCFResourceNodeActor;
class ATCFSquadActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFSquadGatherCommandChanged,
	bool,
	bHasGatherCommand,
	ATCFResourceNodeActor*,
	ResourceNode);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFSquadGatherCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFSquadGatherCommandComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Gather Command")
	bool StartGatherCommand(ATCFResourceNodeActor* NewResourceNode);

	UFUNCTION(BlueprintCallable, Category = "TCF|Gather Command")
	void StopGatherCommand();

	UFUNCTION(BlueprintPure, Category = "TCF|Gather Command")
	bool HasGatherCommand() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Gather Command")
	ATCFResourceNodeActor* GetTargetResourceNode() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Gather Command")
	FOnTCFSquadGatherCommandChanged OnGatherCommandChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Gather Command")
	FGameplayTag GatherOrderTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Gather Command")
	FGameplayTag WorkerRoleTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Gather Command", meta = (ClampMin = "0.05"))
	float CommandUpdateInterval = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Gather Command", meta = (ClampMin = "0.0"))
	float FallbackGatherRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Gather Command", meta = (ClampMin = "0.0"))
	float GatherRangePadding = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Gather Command", meta = (ClampMin = "0.0"))
	float MoveTargetRefreshDistance = 80.0f;

private:
	UPROPERTY()
	TObjectPtr<ATCFSquadActor> SquadOwner;

	UPROPERTY()
	TObjectPtr<ATCFResourceNodeActor> TargetResourceNode;

	FTimerHandle GatherCommandTimerHandle;
	FVector LastMoveTargetLocation = FVector::ZeroVector;
	bool bHasIssuedMove = false;

	void TickGatherCommand();

	bool IsGatherCommandStillValid() const;
	bool IsWorkerSquad() const;
	bool IsTargetInGatherRange() const;

	float GetGatherRange() const;
	FVector GetDesiredGatherMoveLocation() const;

	bool SubmitGatherOrder() const;
	void MoveTowardResourceNode();
	void StopMovement() const;

	void BindResourceNodeDestroyed(ATCFResourceNodeActor* NewResourceNode);
	void UnbindResourceNodeDestroyed();

	UFUNCTION()
	void HandleResourceNodeDestroyed(AActor* DestroyedActor);
};