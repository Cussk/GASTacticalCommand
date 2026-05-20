//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/TCFOrderTypes.h"
#include "TCFOrderSubsystem.generated.h"

class ATCFSquadActor;
class UTCFOrderDefinition;

UCLASS()
class GASTACTICALCOMMAND_API UTCFOrderSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "TCF|Orders")
	FTCFOrderResult SubmitOrder(const FTCFSquadOrderRequest& Request);

	UFUNCTION(BlueprintCallable, Category = "TCF|Orders")
	const UTCFOrderDefinition* GetOrderDefinition(FGameplayTag OrderTag) const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Orders")
	void RebuildOrderDefinitionCache();

private:
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UTCFOrderDefinition>> OrderDefinitionsByTag;

	FTCFOrderResult ValidateOrderRequest(const FTCFSquadOrderRequest& Request, const UTCFOrderDefinition*& OutOrderDefinition) const;

	FTCFOrderResult ValidateSourceTags(const ATCFSquadActor& SourceSquad, const UTCFOrderDefinition& OrderDefinition) const;
	FTCFOrderResult ValidateTarget(const AActor& SourceActor, const FTCFOrderTarget& Target, const UTCFOrderDefinition& OrderDefinition) const;

	static bool DoesSourceHaveRequiredTags(const FGameplayTagContainer& OwnedTags, const FGameplayTagContainer& RequiredTags, FGameplayTagContainer& OutMissingTags);
	static bool DoesSourceHaveBlockedTags(const FGameplayTagContainer& OwnedTags, const FGameplayTagContainer& BlockedTags, FGameplayTagContainer& OutBlockingTags);

	static bool IsTargetTypeValid(const FTCFOrderTarget& Target, const UTCFOrderDefinition& OrderDefinition);
	static bool IsTargetWithinRange(const AActor& SourceActor, const FTCFOrderTarget& Target, const UTCFOrderDefinition& OrderDefinition);
	static bool TryGetTargetLocation(const FTCFOrderTarget& Target, FVector& OutLocation);
};