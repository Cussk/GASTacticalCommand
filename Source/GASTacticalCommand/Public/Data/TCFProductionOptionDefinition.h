//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Types/TCFEconomyTypes.h"
#include "TCFProductionOptionDefinition.generated.h"

class ATCFSquadActor;
class UTCFSquadDefinition;
class UTexture2D;

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFProductionOptionDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	bool IsValidProductionOption() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	UTCFSquadDefinition* GetSquadDefinition() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	TSubclassOf<ATCFSquadActor> GetResolvedSquadActorClass(TSubclassOf<ATCFSquadActor> FallbackClass) const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	float GetSafeProductionTime() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	FText GetSafeDisplayName() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production")
	TObjectPtr<UTCFSquadDefinition> SquadDefinition;

	// Optional override. If unset, the production component's base squad actor class is used.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production")
	TSubclassOf<ATCFSquadActor> SquadActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|Economy")
	TArray<FTCFResourceAmount> Cost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production", meta = (ClampMin = "0.0"))
	float ProductionTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|Requirements")
	FGameplayTagContainer RequiredCommanderTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|Requirements")
	FGameplayTagContainer BlockedCommanderTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|UI")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|UI", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|UI")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production|UI")
	int32 SortPriority = 0;
};