//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/TCFEconomyTypes.h"
#include "TCFResourceNodeActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UTCFResourceNodeDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnTCFResourceNodeAmountChanged,
	ATCFResourceNodeActor*,
	ResourceNode,
	int32,
	OldAmount,
	int32,
	NewAmount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnTCFResourceNodeDepleted,
	ATCFResourceNodeActor*,
	ResourceNode);

UCLASS()
class GASTACTICALCOMMAND_API ATCFResourceNodeActor : public AActor
{
	GENERATED_BODY()

public:
	ATCFResourceNodeActor();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "TCF|Resource Node")
	void ApplyResourceNodeDefinition(UTCFResourceNodeDefinition* NewDefinition, bool bResetAmount = true);

	UFUNCTION(BlueprintCallable, Category = "TCF|Resource Node")
	void InitializeResourceNode();

	UFUNCTION(BlueprintPure, Category = "TCF|Resource Node")
	UTCFResourceNodeDefinition* GetResourceNodeDefinition() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resource Node")
	FGameplayTag GetResourceType() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resource Node")
	int32 GetCurrentAmount() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resource Node")
	int32 GetMaxAmount() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resource Node")
	bool IsDepleted() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resource Node")
	bool CanGatherResource() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Resource Node")
	bool TryGatherResource(int32 RequestedAmount, FTCFResourceAmount& OutGatheredResource);

	UFUNCTION(BlueprintCallable, Category = "TCF|Resource Node")
	void SetCurrentAmount(int32 NewAmount);

	UPROPERTY(BlueprintAssignable, Category = "TCF|Resource Node")
	FOnTCFResourceNodeAmountChanged OnResourceAmountChanged;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Resource Node")
	FOnTCFResourceNodeDepleted OnResourceNodeDepleted;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UStaticMeshComponent> NodeVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<USphereComponent> InteractionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_NodeDefinition, Category = "TCF|Resource Node")
	TObjectPtr<UTCFResourceNodeDefinition> NodeDefinition;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmount, BlueprintReadOnly, Category = "TCF|Resource Node")
	int32 CurrentAmount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "TCF|Resource Node")
	int32 MaxAmount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_Depleted, BlueprintReadOnly, Category = "TCF|Resource Node")
	bool bDepleted = false;

	UFUNCTION()
	void OnRep_NodeDefinition() const;

	UFUNCTION()
	void OnRep_CurrentAmount();

	UFUNCTION()
	void OnRep_Depleted();

private:
	void ApplyDefinitionVisuals() const;
	void HandleDepleted();
	void RestoreFromDepleted();
	void ApplyDepletedPresentation();
	void ScheduleDestroyAfterDepletion();
	void DestroyNode();
};