//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/TCFAffiliationTypes.h"
#include "TCFSelectableHighlightComponent.generated.h"

class UTCFAffiliationComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFSelectableHighlightChanged, bool, bIsSelected);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFSelectableHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFSelectableHighlightComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	void SetSelected(bool bNewSelected);

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool IsSelected() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Selection")
	FOnTCFSelectableHighlightChanged OnSelectionChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "TCF|Selection")
	bool bSelected = false;

	UPROPERTY()
	TObjectPtr<UTCFAffiliationComponent> OwnerAffiliationComponent;

	void ApplySelectionVisualState() const;
	int32 ResolveStencilValueForLocalPlayer() const;
	ETCFSquadRelationship ResolveRelationshipToLocalPlayer() const;

	void BindAffiliationChanges();
	void UnbindAffiliationChanges();

	UFUNCTION()
	void HandleOwnerAffiliationChanged(const FTCFAffiliationData& NewAffiliation);
};