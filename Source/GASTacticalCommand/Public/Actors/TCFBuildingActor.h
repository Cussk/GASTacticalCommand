//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "Data/TCFBuildingDefinition.h"
#include "TCFBuildingActor.generated.h"

class UTCFBuildingProductionComponent;
class UTCFSelectableHighlightComponent;
struct FOnAttributeChangeData;
class UBoxComponent;
class UStaticMeshComponent;
class UTCFAffiliationComponent;
class UAbilitySystemComponent;
class UTCFBuildingAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFBuildingRuntimeStateChanged,
	ETCFBuildingRuntimeState,
	OldState,
	ETCFBuildingRuntimeState,
	NewState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnTCFBuildingConstructionProgressChanged,
	ATCFBuildingActor*,
	Building,
	float,
	OldProgress,
	float,
	NewProgress);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFBuildingConstructionCompleted,
	ATCFBuildingActor*,
	Building,
	AActor*,
	CompletionSource);

UCLASS()
class GASTACTICALCOMMAND_API ATCFBuildingActor : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATCFBuildingActor();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "TCF|Building")
	void ApplyBuildingDefinition(UTCFBuildingDefinition* NewDefinition, bool bApplyInitialRuntimeState = true);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "TCF|Components")
	UTCFBuildingAttributeSet* GetBuildingAttributeSet() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	UTCFBuildingDefinition* GetBuildingDefinition() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	FGameplayTag GetBuildingTypeTag() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	FGameplayTagContainer GetBuildingRoleTags() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	FIntPoint GetFootprintSize() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	ETCFBuildingRuntimeState GetRuntimeState() const;
	
	UFUNCTION(BlueprintPure, Category = "TCF|Building|Interaction")
	UPrimitiveComponent* GetInteractionCollisionComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Components")
	UTCFSelectableHighlightComponent* GetSelectionHighlightComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Components")
	UTCFBuildingProductionComponent* GetProductionComponent() const;
	
	UFUNCTION(BlueprintPure, Category = "TCF|Building|Placement")
	bool HasReservedPlacementFootprint() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building|Placement")
	FIntPoint GetReservedPlacementAnchorCell() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	bool IsActive() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Building")
	void SetRuntimeState(ETCFBuildingRuntimeState NewRuntimeState);
	
	UFUNCTION(BlueprintPure, Category = "TCF|Building|Construction")
	bool IsUnderConstruction() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building|Construction")
	bool IsConstructionComplete() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building|Construction")
	bool CanReceiveConstructionWork() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building|Construction")
	float GetConstructionWorkCompleted() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building|Construction")
	float GetRequiredConstructionWork() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building|Construction")
	float GetConstructionProgressAlpha() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Building|Construction")
	bool AddConstructionWork(float WorkAmount, AActor* WorkSource);

	UFUNCTION(BlueprintCallable, Category = "TCF|Building|Construction")
	void CompleteConstruction(AActor* CompletionSource);

	UFUNCTION(BlueprintCallable, Category = "TCF|Components")
	UTCFAffiliationComponent* GetAffiliationComponent() const;
	
	UFUNCTION(BlueprintPure, Category = "TCF|Building|Health")
	bool IsDestroyed() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building|Health")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Building|Health")
	void HandleBuildingHealthDepleted(AActor* DamageSource);

	UPROPERTY(BlueprintAssignable, Category = "TCF|Building")
	FOnTCFBuildingRuntimeStateChanged OnRuntimeStateChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "TCF|Building|Construction")
	FOnTCFBuildingConstructionProgressChanged OnConstructionProgressChanged;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Building|Construction")
	FOnTCFBuildingConstructionCompleted OnConstructionCompleted;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UStaticMeshComponent> BuildingVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UBoxComponent> InteractionCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFAffiliationComponent> AffiliationComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFSelectableHighlightComponent> SelectionHighlightComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFBuildingProductionComponent> ProductionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|GAS")
	TObjectPtr<UTCFBuildingAttributeSet> BuildingAttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BuildingDefinition, Category = "TCF|Building")
	TObjectPtr<UTCFBuildingDefinition> BuildingDefinition;

	UPROPERTY(ReplicatedUsing = OnRep_RuntimeState, BlueprintReadOnly, Category = "TCF|Building")
	ETCFBuildingRuntimeState RuntimeState = ETCFBuildingRuntimeState::Inactive;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Building|Placement")
	bool bReservePlacementGridOnBeginPlay = true;
	
	UPROPERTY(ReplicatedUsing = OnRep_ConstructionWorkCompleted, BlueprintReadOnly, Category = "TCF|Building|Construction")
	float ConstructionWorkCompleted = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Building|Death")
	bool bDestroyActorOnHealthDepleted = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Building|Death", meta = (ClampMin = "0.0"))
	float DestroyDelayAfterHealthDepleted = 0.0f;

	UFUNCTION()
	void OnRep_BuildingDefinition();

	UFUNCTION()
	void OnRep_RuntimeState(ETCFBuildingRuntimeState OldRuntimeState);
	
	UFUNCTION()
	void OnRep_ConstructionWorkCompleted(float OldConstructionWorkCompleted);

private:
	FIntPoint ReservedPlacementAnchorCell = FIntPoint::ZeroValue;
	bool bHasReservedPlacementFootprint = false;
	
	bool bHasHandledHealthDepleted = false;
	
	void InitializeAbilitySystem();
	void InitializeAttributesFromDefinition() const;
	
	void InitializeFromDefinition();
	void ApplyDefinitionVisuals() const;
	void ApplyRuntimeStatePresentation();
	
	void InitializeConstructionStateFromDefinition();
	void SetConstructionWorkCompleted(float NewConstructionWorkCompleted);

	void TryReservePlacementFootprint();
	void ReleasePlacementFootprint();
	
	void BindAttributeChangeDelegates();
	void HandleHealthAttributeChanged(const FOnAttributeChangeData& ChangeData);

	void ApplyVisualForCurrentRuntimeState() const;
	FVector ResolveVisualScaleForCurrentRuntimeState() const;

	void DestroyBuildingActor();
};