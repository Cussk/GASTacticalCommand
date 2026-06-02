// Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "Types/TCFBuildingConstructionTypes.h"
#include "TCFPlayerConstructionComponent.generated.h"

class ATCFBuildingActor;
class ATCFPlayerState;
class UAbilitySystemComponent;
class UGameplayAbility;
class UTCFPlayerResourceBankComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnTCFConstructionSitePlaced,
	ATCFBuildingActor*,
	PlacedBuilding,
	UTCFBuildingDefinition*,
	BuildingDefinition,
	FVector,
	PlacementLocation,
	FIntPoint,
	AnchorCell);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFPlayerConstructionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFPlayerConstructionComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "TCF|Construction")
	void GrantConstructionAbility();

	UFUNCTION(BlueprintCallable, Category = "TCF|Construction")
	bool TryRequestBuildingConstruction(
		UTCFBuildingDefinition* BuildingDefinition,
		FVector PlacementLocation,
		FIntPoint AnchorCell,
		const FTCFPlacementGridValidationResult& PlacementValidationResult,
		ATCFBuildingActor*& OutPlacedBuilding);

	UFUNCTION(BlueprintPure, Category = "TCF|Construction")
	bool HasPendingConstructionRequest() const;

	const FTCFBuildingConstructionRequest& GetPendingConstructionRequest() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Construction")
	void ClearPendingConstructionRequest();

	bool CanExecutePendingConstructionRequest(FTCFPlacementGridValidationResult* OutValidationResult = nullptr) const;
	bool ExecutePendingConstruction(ATCFBuildingActor*& OutPlacedBuilding);
	void RefundPendingConstructionCost() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Construction")
	ATCFBuildingActor* GetLastPlacedBuilding() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Construction")
	UTCFPlayerResourceBankComponent* GetResourceBankComponent() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Construction")
	FOnTCFConstructionSitePlaced OnConstructionSitePlaced;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction")
	TSubclassOf<UGameplayAbility> ConstructBuildingAbilityClass;

private:
	UPROPERTY()
	TObjectPtr<ATCFBuildingActor> LastPlacedBuilding;

	FTCFBuildingConstructionRequest PendingConstructionRequest;
	FGameplayAbilitySpecHandle ConstructionAbilitySpecHandle;

	ATCFPlayerState* GetTCFPlayerState() const;
	UAbilitySystemComponent* GetCommanderAbilitySystemComponent() const;

	void ApplyPlacedBuildingAffiliation(ATCFBuildingActor* PlacedBuilding) const;
};