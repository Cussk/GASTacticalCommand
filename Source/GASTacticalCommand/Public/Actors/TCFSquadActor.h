//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "TCFSquadActor.generated.h"

class UTCFSquadMovementComponent;
class UTCFAffiliationComponent;
class UTCFSquadSelectionComponent;
class UAbilitySystemComponent;
class UGameplayEffect;
class USceneComponent;
class UStaticMeshComponent;
class UTCFSquadAttributeSet;
class UTCFSquadDefinition;

UCLASS()
class GASTACTICALCOMMAND_API ATCFSquadActor : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATCFSquadActor();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Components")
	UTCFSquadMovementComponent* GetMovementComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Squad")
	UTCFSquadSelectionComponent* GetSquadSelectionComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Components")
	UTCFAffiliationComponent* GetAffiliationComponent() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Squad")
	const UTCFSquadDefinition* GetSquadDefinition() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Squad")
	const UTCFSquadAttributeSet* GetSquadAttributeSet() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Squad")
	FGameplayTag GetRoleTag() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Squad")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Squad")
	float GetSelectionRadius() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Squad")
	bool IsInitialized() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UStaticMeshComponent> SquadVisual;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFSquadMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFSquadSelectionComponent> SquadSelectionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFAffiliationComponent> AffiliationComponent;

	UPROPERTY()
	TObjectPtr<UTCFSquadAttributeSet> SquadAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Squad")
	TObjectPtr<UTCFSquadDefinition> SquadDefinition;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "TCF|Squad")
	bool bInitialized = false;

	void InitializeFromDefinition();
	void InitializeAttributesFromDefinition() const;
	void GrantStartupTags() const;
	void GrantStartupEffects() const;
	void GrantStartupAbilities() const;

	bool CanInitializeFromDefinition() const;
};
