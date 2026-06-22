//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "TCFResourceUIDefinition.generated.h"

USTRUCT(BlueprintType)
struct FTCFResourceUIViewData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resources")
	FGameplayTag ResourceTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resources")
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resources")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resources")
	FSlateBrush IconBrush;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resources")
	TObjectPtr<UTexture2D> Icon;
};

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFResourceUIDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Resources|UI")
	bool TryGetResourceViewData(FGameplayTag ResourceTag, FTCFResourceUIViewData& OutViewData) const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|Resources|UI")
	void GetAllResourceViewData(TArray<FTCFResourceUIViewData>& OutResourceViewData) const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resources|UI")
	FText GetResourceDisplayName(FGameplayTag ResourceTag) const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resources|UI")
	FSlateBrush GetResourceIconBrush(FGameplayTag ResourceTag) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resources|UI")
	TArray<FTCFResourceUIViewData> ResourceViewData;
};