//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/TCFOrderTypes.h"
#include "TCFOrderPayload.generated.h"

class UTCFOrderDefinition;

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFOrderPayload : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const FTCFSquadOrderRequest& InRequest, UTCFOrderDefinition* InOrderDefinition);

	UFUNCTION(BlueprintPure, Category = "TCF|Orders")
	const FTCFSquadOrderRequest& GetRequest() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Orders")
	UTCFOrderDefinition* GetOrderDefinition() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Orders")
	bool IsValidPayload() const;

private:
	UPROPERTY()
	FTCFSquadOrderRequest Request;

	UPROPERTY()
	TObjectPtr<UTCFOrderDefinition> OrderDefinition;
};
