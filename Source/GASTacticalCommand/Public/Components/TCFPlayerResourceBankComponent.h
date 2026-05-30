//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/TCFEconomyTypes.h"
#include "TCFPlayerResourceBankComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnTCFResourceAmountChanged,
	FGameplayTag,
	ResourceType,
	int32,
	OldAmount,
	int32,
	NewAmount);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFPlayerResourceBankComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFPlayerResourceBankComponent();

	UFUNCTION(BlueprintPure, Category = "TCF|Economy")
	int32 GetResourceAmount(FGameplayTag ResourceType) const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Economy")
	void SetResourceAmount(FGameplayTag ResourceType, int32 NewAmount);

	UFUNCTION(BlueprintCallable, Category = "TCF|Economy")
	void AddResourceAmount(FGameplayTag ResourceType, int32 AmountToAdd);

	UFUNCTION(BlueprintCallable, Category = "TCF|Economy")
	bool CanAffordResources(const TArray<FTCFResourceAmount>& Costs) const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Economy")
	bool TrySpendResources(const TArray<FTCFResourceAmount>& Costs, FTCFResourceTransactionResult& OutResult);

	UFUNCTION(BlueprintCallable, Category = "TCF|Economy")
	void AddResources(const TArray<FTCFResourceAmount>& ResourceAmounts);

	UFUNCTION(BlueprintPure, Category = "TCF|Economy")
	const TMap<FGameplayTag, int32>& GetResourceBalances() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Economy")
	FOnTCFResourceAmountChanged OnResourceAmountChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Economy")
	TArray<FTCFResourceAmount> StartingResources;

private:
	UPROPERTY()
	TMap<FGameplayTag, int32> ResourceBalances;

	void ApplyStartingResources();
	bool IsValidResourceAmount(const FTCFResourceAmount& ResourceAmount) const;
};