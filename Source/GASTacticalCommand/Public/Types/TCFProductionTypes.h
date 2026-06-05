//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "TCFProductionTypes.generated.h"

class ATCFPlayerState;
class UTCFProductionOptionDefinition;

USTRUCT(BlueprintType)
struct FTCFProductionRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	TObjectPtr<UTCFProductionOptionDefinition> ProductionOption = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Production")
	TObjectPtr<ATCFPlayerState> RequestingPlayerState = nullptr;

	bool IsValid() const
	{
		return ProductionOption && RequestingPlayerState;
	}
};