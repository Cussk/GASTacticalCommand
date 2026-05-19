//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCFSquadSelectionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFSquadSelectionChanged, bool, bIsSelected);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFSquadSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFSquadSelectionComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	void SetSelected(bool bNewSelected);

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	bool IsSelected() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Selection")
	FOnTCFSquadSelectionChanged OnSelectionChanged;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "TCF|Selection")
	bool bSelected = false;

	void ApplySelectionVisualState() const;
};