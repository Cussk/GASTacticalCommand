//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TCFSquadBuildCommandComponent.generated.h"

class ATCFBuildingActor;
class ATCFSquadActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFSquadBuildCommandChanged,
	bool,
	bHasBuildCommand,
	ATCFBuildingActor*,
	Building);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFSquadBuildCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFSquadBuildCommandComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Build Command")
	bool StartBuildCommand(ATCFBuildingActor* NewBuilding);

	UFUNCTION(BlueprintCallable, Category = "TCF|Build Command")
	void StopBuildCommand();

	UFUNCTION(BlueprintPure, Category = "TCF|Build Command")
	bool HasBuildCommand() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Build Command")
	ATCFBuildingActor* GetTargetBuilding() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Build Command")
	FOnTCFSquadBuildCommandChanged OnBuildCommandChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Build Command")
	FGameplayTag BuildOrderTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Build Command")
	FGameplayTag WorkerRoleTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Build Command", meta = (ClampMin = "0.05"))
	float CommandUpdateInterval = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Build Command", meta = (ClampMin = "0.0"))
	float FallbackBuildRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Build Command", meta = (ClampMin = "0.0"))
	float BuildRangePadding = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Build Command", meta = (ClampMin = "0.0"))
	float MoveTargetRefreshDistance = 80.0f;

private:
	UPROPERTY()
	TObjectPtr<ATCFSquadActor> SquadOwner;

	UPROPERTY()
	TObjectPtr<ATCFBuildingActor> TargetBuilding;

	FTimerHandle BuildCommandTimerHandle;
	FVector LastMoveTargetLocation = FVector::ZeroVector;
	bool bHasIssuedMove = false;

	void TickBuildCommand();

	bool IsBuildCommandStillValid() const;
	bool IsWorkerSquad() const;
	bool IsBuildTargetRelationshipAllowed() const;
	bool IsTargetInBuildRange() const;

	float GetBuildRange() const;
	FVector GetDesiredBuildMoveLocation() const;

	bool SubmitBuildOrder() const;
	void MoveTowardBuilding();
	void StopMovement() const;

	void BindBuildingDestroyed(ATCFBuildingActor* NewBuilding);
	void UnbindBuildingDestroyed();

	UFUNCTION()
	void HandleBuildingDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void HandleBuildingConstructionCompleted(ATCFBuildingActor* Building, AActor* CompletionSource);
};