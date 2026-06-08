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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnTCFProductionQueueChanged,
	UTCFBuildingProductionComponent*,
	ProductionComponent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFProductionProgressChanged,
	UTCFBuildingProductionComponent*,
	ProductionComponent,
	float,
	ActiveProgressAlpha);

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
	void RefundPendingProductionCost();

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	UTCFPlayerResourceBankComponent* GetResourceBankForPendingRequest() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void InitializeProductionAbility();
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	bool EnqueuePendingProduction();

	UFUNCTION(BlueprintPure, Category = "TCF|Production|Queue")
	bool HasProductionQueueSpace() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production|Queue")
	bool HasQueuedProduction() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production|Queue")
	int32 GetProductionQueueCount() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production|Queue")
	int32 GetEffectiveMaxQueueSize() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Production|Queue")
	void GetProductionQueue(TArray<FTCFProductionQueueItem>& OutQueue) const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production|Queue")
	bool TryGetActiveProductionItem(FTCFProductionQueueItem& OutQueueItem) const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production|Queue")
	float GetActiveProductionProgressAlpha() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Production")
	FOnTCFProducedSquadSpawned OnProducedSquadSpawned;
	
	UPROPERTY(BlueprintAssignable, Category = "TCF|Production")
	FOnTCFProductionQueueChanged OnProductionQueueChanged;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Production")
	FOnTCFProductionProgressChanged OnProductionProgressChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Production|GAS")
	TSubclassOf<UGameplayAbility> ProductionAbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Production")
	TSubclassOf<ATCFSquadActor> DefaultSquadActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Production", meta = (ClampMin = "0.0"))
	float SpawnPadding = 150.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Production|Queue", meta = (ClampMin = "1"))
	int32 MaxQueueSize = 5;
	
	// Modify from research and faction bonuses later
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Production|Queue", meta = (ClampMin = "1"))
	int32 BonusQueueSize = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Production|Queue", meta = (ClampMin = "0.05"))
	float ProductionUpdateInterval = 0.25f;

private:
	UPROPERTY()
	TObjectPtr<ATCFBuildingActor> BuildingOwner;
	
	UPROPERTY()
	FTCFProductionRequest PendingProductionRequest;
	
	UPROPERTY()
	TArray<FTCFProductionQueueItem> ProductionQueue;

	FGameplayAbilitySpecHandle ProductionAbilityHandle;
	FTimerHandle ProductionTimerHandle;

	UTCFProductionCatalogDefinition* GetProductionCatalog() const;
	bool IsRequesterOwner(const ATCFPlayerState* RequestingPlayerState) const;
	FTransform ResolveSpawnTransform(const UTCFProductionOptionDefinition& ProductionOption) const;
	void ApplySpawnedSquadAffiliation(ATCFSquadActor& Squad, const ATCFPlayerState& RequestingPlayerState) const;
	
	bool TryActivateProductionAbility();
	bool DoesRequesterMeetCommanderTagRequirements(const UTCFProductionOptionDefinition& ProductionOption, const ATCFPlayerState& RequestingPlayerState) const;
	
	bool TrySpawnProducedSquad(UTCFProductionOptionDefinition* ProductionOption, ATCFPlayerState* RequestingPlayerState, ATCFSquadActor*& OutSquad);

	void StartProductionTimerIfNeeded();
	void StopProductionTimer();
	void HandleProductionTimerTick();
	void AdvanceActiveProduction(float DeltaSeconds);
	void CompleteActiveProduction();

	bool SpawnProductionQueueItem(
		const FTCFProductionQueueItem& QueueItem,
		ATCFSquadActor*& OutSquad);

	float GetProductionRateMultiplier(const FTCFProductionQueueItem& QueueItem) const;
};