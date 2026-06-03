// Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TCFCommanderBuildCatalogDefinition.generated.h"

class UTCFConstructionOptionDefinition;

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFCommanderBuildCatalogDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Construction")
	void GetConstructionOptions(TArray<UTCFConstructionOptionDefinition*>& OutOptions) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Construction")
	TArray<TObjectPtr<UTCFConstructionOptionDefinition>> ConstructionOptions;
};