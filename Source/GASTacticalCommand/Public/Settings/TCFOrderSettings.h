//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TCFOrderSettings.generated.h"

class UTCFOrderDefinition;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "TCF Orders"))
class GASTACTICALCOMMAND_API UTCFOrderSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "TCF|Orders")
	TArray<TSoftObjectPtr<UTCFOrderDefinition>> OrderDefinitions;
};