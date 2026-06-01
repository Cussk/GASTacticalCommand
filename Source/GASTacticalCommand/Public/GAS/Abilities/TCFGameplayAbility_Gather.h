//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/TCFGameplayAbility_OrderBase.h"
#include "TCFGameplayAbility_Gather.generated.h"

class ATCFPlayerState;
class ATCFResourceNodeActor;
class ATCFSquadActor;
class UTCFPlayerResourceBankComponent;

UCLASS()
class GASTACTICALCOMMAND_API UTCFGameplayAbility_Gather : public UTCFGameplayAbility_OrderBase
{
	GENERATED_BODY()

public:
	UTCFGameplayAbility_Gather();

protected:
	virtual void HandleOrderActivated() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Gather")
	bool bRequireWorkerRole = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Gather", meta = (ClampMin = "1"))
	int32 MinimumGatherAmount = 1;

private:
	ATCFResourceNodeActor* GetTargetResourceNode() const;
	bool IsValidWorker(const ATCFSquadActor& SourceSquad) const;
	int32 CalculateGatherAmount(const ATCFSquadActor& SourceSquad, const ATCFResourceNodeActor& ResourceNode) const;

	ATCFPlayerState* ResolvePlayerStateForSource(const ATCFSquadActor& SourceSquad) const;
	UTCFPlayerResourceBankComponent* ResolveResourceBankForSource(const ATCFSquadActor& SourceSquad) const;
};