//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/TCFBuildingDefinition.h"
#include "TCFBuildingActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UTCFAffiliationComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFBuildingRuntimeStateChanged,
	ETCFBuildingRuntimeState,
	OldState,
	ETCFBuildingRuntimeState,
	NewState);

UCLASS()
class GASTACTICALCOMMAND_API ATCFBuildingActor : public AActor
{
	GENERATED_BODY()

public:
	ATCFBuildingActor();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "TCF|Building")
	void ApplyBuildingDefinition(UTCFBuildingDefinition* NewDefinition, bool bApplyInitialRuntimeState = true);

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	UTCFBuildingDefinition* GetBuildingDefinition() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	FGameplayTag GetBuildingTypeTag() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	FGameplayTagContainer GetBuildingRoleTags() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	const TArray<FTCFResourceAmount>& GetCost() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	FIntPoint GetFootprintSize() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	ETCFBuildingRuntimeState GetRuntimeState() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	bool IsActive() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Building")
	void SetRuntimeState(ETCFBuildingRuntimeState NewRuntimeState);

	UFUNCTION(BlueprintCallable, Category = "TCF|Components")
	UTCFAffiliationComponent* GetAffiliationComponent() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Building")
	FOnTCFBuildingRuntimeStateChanged OnRuntimeStateChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UStaticMeshComponent> BuildingVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UBoxComponent> InteractionCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFAffiliationComponent> AffiliationComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_BuildingDefinition, Category = "TCF|Building")
	TObjectPtr<UTCFBuildingDefinition> BuildingDefinition;

	UPROPERTY(ReplicatedUsing = OnRep_RuntimeState, BlueprintReadOnly, Category = "TCF|Building")
	ETCFBuildingRuntimeState RuntimeState = ETCFBuildingRuntimeState::Inactive;

	UFUNCTION()
	void OnRep_BuildingDefinition();

	UFUNCTION()
	void OnRep_RuntimeState(ETCFBuildingRuntimeState OldRuntimeState);

private:
	void InitializeFromDefinition();
	void ApplyDefinitionVisuals() const;
	void ApplyRuntimeStatePresentation();
};