//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFPlayerResourceBankComponent.h"

UTCFPlayerResourceBankComponent::UTCFPlayerResourceBankComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFPlayerResourceBankComponent::BeginPlay()
{
	Super::BeginPlay();

	ApplyStartingResources();
}

int32 UTCFPlayerResourceBankComponent::GetResourceAmount(FGameplayTag ResourceType) const
{
	if (!ResourceType.IsValid())
	{
		return 0;
	}

	const int32* FoundAmount = ResourceBalances.Find(ResourceType);
	return FoundAmount ? *FoundAmount : 0;
}

void UTCFPlayerResourceBankComponent::SetResourceAmount(FGameplayTag ResourceType, int32 NewAmount)
{
	if (!ResourceType.IsValid())
	{
		return;
	}

	const int32 ClampedAmount = FMath::Max(0, NewAmount);
	const int32 OldAmount = GetResourceAmount(ResourceType);

	if (OldAmount == ClampedAmount)
	{
		return;
	}

	ResourceBalances.FindOrAdd(ResourceType) = ClampedAmount;
	OnResourceAmountChanged.Broadcast(ResourceType, OldAmount, ClampedAmount);
}

void UTCFPlayerResourceBankComponent::AddResourceAmount(FGameplayTag ResourceType, int32 AmountToAdd)
{
	if (!ResourceType.IsValid() || AmountToAdd == 0)
	{
		return;
	}

	const int32 CurrentAmount = GetResourceAmount(ResourceType);
	SetResourceAmount(ResourceType, CurrentAmount + AmountToAdd);
}

bool UTCFPlayerResourceBankComponent::CanAffordResources(const TArray<FTCFResourceAmount>& Costs) const
{
	for (const FTCFResourceAmount& Cost : Costs)
	{
		if (!IsValidResourceAmount(Cost))
		{
			continue;
		}

		if (GetResourceAmount(Cost.ResourceType) < Cost.Amount)
		{
			return false;
		}
	}

	return true;
}

bool UTCFPlayerResourceBankComponent::TrySpendResources(
	const TArray<FTCFResourceAmount>& Costs,
	FTCFResourceTransactionResult& OutResult)
{
	for (const FTCFResourceAmount& Cost : Costs)
	{
		if (!IsValidResourceAmount(Cost))
		{
			continue;
		}

		const int32 CurrentAmount = GetResourceAmount(Cost.ResourceType);
		if (CurrentAmount < Cost.Amount)
		{
			OutResult = FTCFResourceTransactionResult::Failure(
				Cost.ResourceType,
				CurrentAmount,
				Cost.Amount);

			return false;
		}
	}

	for (const FTCFResourceAmount& Cost : Costs)
	{
		if (!IsValidResourceAmount(Cost))
		{
			continue;
		}

		AddResourceAmount(Cost.ResourceType, -Cost.Amount);
	}

	OutResult = FTCFResourceTransactionResult::Success();
	return true;
}

void UTCFPlayerResourceBankComponent::AddResources(const TArray<FTCFResourceAmount>& ResourceAmounts)
{
	for (const FTCFResourceAmount& ResourceAmount : ResourceAmounts)
	{
		if (!IsValidResourceAmount(ResourceAmount))
		{
			continue;
		}

		AddResourceAmount(ResourceAmount.ResourceType, ResourceAmount.Amount);
	}
}

const TMap<FGameplayTag, int32>& UTCFPlayerResourceBankComponent::GetResourceBalances() const
{
	return ResourceBalances;
}

void UTCFPlayerResourceBankComponent::ApplyStartingResources()
{
	for (const FTCFResourceAmount& StartingResource : StartingResources)
	{
		if (!IsValidResourceAmount(StartingResource))
		{
			continue;
		}

		SetResourceAmount(StartingResource.ResourceType, StartingResource.Amount);
	}
}

bool UTCFPlayerResourceBankComponent::IsValidResourceAmount(const FTCFResourceAmount& ResourceAmount) const
{
	return ResourceAmount.ResourceType.IsValid()
		&& ResourceAmount.Amount > 0;
}