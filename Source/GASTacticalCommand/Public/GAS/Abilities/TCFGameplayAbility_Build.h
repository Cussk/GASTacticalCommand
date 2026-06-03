//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/TCFGameplayAbility_OrderBase.h"
#include "TCFGameplayAbility_Build.generated.h"

class ATCFBuildingActor;
class ATCFSquadActor;

UCLASS()
class GASTACTICALCOMMAND_API UTCFGameplayAbility_Build : public UTCFGameplayAbility_OrderBase
{
	GENERATED_BODY()

public:
	UTCFGameplayAbility_Build();

protected:
	virtual bool CanActivateCurrentOrder() const override;
	virtual void HandleOrderActivated() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Build")
	bool bRequireWorkerRole = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Build", meta = (ClampMin = "0.0"))
	float BaseConstructionWorkPerActivation = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Build", meta = (ClampMin = "0.0"))
	float MinimumConstructionWork = 1.0f;

private:
	ATCFBuildingActor* GetTargetBuilding() const;
	bool IsValidWorker(const ATCFSquadActor& SourceSquad) const;
	bool IsBuildTargetRelationshipAllowed(const ATCFSquadActor& SourceSquad, const ATCFBuildingActor& TargetBuilding) const;
	float CalculateConstructionWork(const ATCFSquadActor& SourceSquad) const;
};