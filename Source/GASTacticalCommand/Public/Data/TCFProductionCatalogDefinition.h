//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TCFProductionCatalogDefinition.generated.h"

class UTCFProductionOptionDefinition;

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFProductionCatalogDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Production")
	void GetProductionOptions(TArray<UTCFProductionOptionDefinition*>& OutOptions) const;

	UFUNCTION(BlueprintPure, Category = "TCF|Production")
	bool ContainsProductionOption(const UTCFProductionOptionDefinition* ProductionOption) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Production")
	TArray<TObjectPtr<UTCFProductionOptionDefinition>> ProductionOptions;
};