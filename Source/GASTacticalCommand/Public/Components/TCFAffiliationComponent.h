//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/TCFAffiliationTypes.h"
#include "TCFAffiliationComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFAffiliationChanged, const FTCFAffiliationData&, NewAffiliation);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFAffiliationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFAffiliationComponent();

	const FTCFAffiliationData& GetAffiliation() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Affiliation")
	FTCFAffiliationData GetAffiliationForBlueprint() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Affiliation")
	void SetAffiliation(const FTCFAffiliationData& NewAffiliation);

	UFUNCTION(BlueprintCallable, Category = "TCF|Affiliation")
	void SetOwnerId(int32 NewOwnerId);

	UFUNCTION(BlueprintCallable, Category = "TCF|Affiliation")
	void SetTeamId(int32 NewTeamId);

	UFUNCTION(BlueprintCallable, Category = "TCF|Affiliation")
	void SetFactionTag(FGameplayTag NewFactionTag);

	UPROPERTY(BlueprintAssignable, Category = "TCF|Affiliation")
	FOnTCFAffiliationChanged OnAffiliationChanged;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Affiliation", meta = (AllowPrivateAccess = "true"))
	FTCFAffiliationData Affiliation;

	void BroadcastAffiliationChanged();
};
