//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/TCFAffiliationTypes.h"
#include "TCFRelationshipSubsystem.generated.h"

class UTCFAffiliationComponent;

UCLASS()
class GASTACTICALCOMMAND_API UTCFRelationshipSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Relationships")
	ETCFSquadRelationship GetActorRelationship(const AActor* SourceActor, const AActor* OtherActor) const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Relationships")
	ETCFSquadRelationship GetAffiliationRelationship(const FTCFAffiliationData& SourceAffiliation, const FTCFAffiliationData& OtherAffiliation) const;

private:
	static const UTCFAffiliationComponent* FindAffiliationComponent(const AActor* Actor);
};