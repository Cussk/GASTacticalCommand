//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/TCFAffiliationTypes.h"
#include "TCFSquadQuerySubsystem.generated.h"

class ATCFSquadActor;

UCLASS()
class GASTACTICALCOMMAND_API UTCFSquadQuerySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Squads")
	void RegisterSquad(ATCFSquadActor* Squad);

	UFUNCTION(BlueprintCallable, Category = "TCF|Squads")
	void UnregisterSquad(ATCFSquadActor* Squad);

	UFUNCTION(BlueprintCallable, Category = "TCF|Squads")
	void GetSquadsInRadius(
		FVector Origin,
		float Radius,
		const ATCFSquadActor* IgnoredSquad,
		TArray<ATCFSquadActor*>& OutSquads);

	UFUNCTION(BlueprintCallable, Category = "TCF|Squads")
	void GetSquadsInRadiusByRelationship(
		FVector Origin,
		float Radius,
		const AActor* SourceActor,
		const FTCFSquadQueryRelationshipFilter& RelationshipFilter,
		TArray<ATCFSquadActor*>& OutSquads);

private:
	TSet<TWeakObjectPtr<ATCFSquadActor>> RegisteredSquads;
	
	void GatherSquadsInRadius(const FVector& Origin, float Radius, TArray<ATCFSquadActor*>& OutSquads);

	void CompactInvalidSquads();
};
