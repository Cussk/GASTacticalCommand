//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Types/TCFOrderTypes.h"
#include "TCFOrderDefinition.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFOrderDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Identity")
	FName OrderId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Identity", meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Tags")
	FGameplayTag OrderTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Targeting")
	FTCFOrderTargetingConfig Targeting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Presentation")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Debug")
	bool bShowInDebugOrderList = true;

	bool IsValidDefinition() const
	{
		return !OrderId.IsNone() && OrderTag.IsValid();
	}
};
