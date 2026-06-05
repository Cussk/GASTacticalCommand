//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/ActorComponent.h"
#include "Types/TCFProductionTypes.h"
#include "TCFBuildingProductionComponent.generated.h"

class ATCFBuildingActor;
class ATCFPlayerState;
class ATCFSquadActor;
class UGameplayAbility;
class UTCFPlayerResourceBankComponent;
class UTCFProductionCatalogDefinition;
class UTCFProductionOptionDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFProducedSquadSpawned,
	ATCFSquadActor*,
	SpawnedSquad,
	UTCFProductionOptionDefinition*,
	ProductionOption);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFBuildingProductionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFBuildingProductionComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void GetProductionOptions(TArray<UTCFProductionOptionDefinition*>& OutOptions) const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	bool CanUseProductionOption(
		UTCFProductionOptionDefinition* ProductionOption,
		ATCFPlayerState* RequestingPlayerState) const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	bool RequestProduction(
		UTCFProductionOptionDefinition* ProductionOption,
		ATCFPlayerState* RequestingPlayerState);

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	bool HasPendingProductionRequest() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	const FTCFProductionRequest& GetPendingProductionRequest() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void ClearPendingProductionRequest();

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	bool CanExecutePendingProductionRequest() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	bool ExecutePendingProduction(ATCFSquadActor*& OutSquad);

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void RefundPendingProductionCost();

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	UTCFPlayerResourceBankComponent* GetResourceBankForPendingRequest() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void InitializeProductionAbility();

	UPROPERTY(BlueprintAssignable, Category = "TCF|Production")
	FOnTCFProducedSquadSpawned OnProducedSquadSpawned;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Production|GAS")
	TSubclassOf<UGameplayAbility> ProductionAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Production")
	TSubclassOf<ATCFSquadActor> DefaultSquadActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Production", meta = (ClampMin = "0.0"))
	float SpawnPadding = 150.0f;

private:
	UPROPERTY()
	TObjectPtr<ATCFBuildingActor> BuildingOwner;
	
	UPROPERTY()
	FTCFProductionRequest PendingProductionRequest;

	FGameplayAbilitySpecHandle ProductionAbilityHandle;

	UTCFProductionCatalogDefinition* GetProductionCatalog() const;
	bool IsRequesterOwner(const ATCFPlayerState* RequestingPlayerState) const;
	FTransform ResolveSpawnTransform(const UTCFProductionOptionDefinition& ProductionOption) const;
	void ApplySpawnedSquadAffiliation(ATCFSquadActor& Squad, const ATCFPlayerState& RequestingPlayerState) const;
	
	bool TryActivateProductionAbility();
	bool DoesRequesterMeetCommanderTagRequirements(const UTCFProductionOptionDefinition& ProductionOption, const ATCFPlayerState& RequestingPlayerState) const;
	
	bool TrySpawnProducedSquad(UTCFProductionOptionDefinition* ProductionOption, ATCFPlayerState* RequestingPlayerState, ATCFSquadActor*& OutSquad);
};