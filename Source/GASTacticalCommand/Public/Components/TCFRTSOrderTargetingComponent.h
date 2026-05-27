//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/TCFOrderTypes.h"
#include "TCFRTSOrderTargetingComponent.generated.h"

class APlayerController;
class UDecalComponent;
class UMaterialInterface;
class UTCFOrderDefinition;
class UTCFPlayerOrderComponent;
class UTCFPlayerSelectionComponent;
class UTCFRTSHoverContextComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFOrderTargetingStarted, UTCFOrderDefinition*, OrderDefinition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTCFOrderTargetingCanceled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFOrderTargetingConfirmed, const FTCFOrderTarget&, Target);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFRTSOrderTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFRTSOrderTargetingComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Order Targeting")
	bool BeginOrderTargeting(UTCFOrderDefinition* OrderDefinition);

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Order Targeting")
	void CancelOrderTargeting();

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Order Targeting")
	bool ConfirmPendingOrder();

	UFUNCTION(BlueprintPure, Category = "TCF|RTS Order Targeting")
	bool HasPendingOrder() const;

	UFUNCTION(BlueprintPure, Category = "TCF|RTS Order Targeting")
	UTCFOrderDefinition* GetPendingOrderDefinition() const;

	UFUNCTION(BlueprintPure, Category = "TCF|RTS Order Targeting")
	bool IsCurrentTargetValid() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|RTS Order Targeting")
	FOnTCFOrderTargetingStarted OnOrderTargetingStarted;

	UPROPERTY(BlueprintAssignable, Category = "TCF|RTS Order Targeting")
	FOnTCFOrderTargetingCanceled OnOrderTargetingCanceled;

	UPROPERTY(BlueprintAssignable, Category = "TCF|RTS Order Targeting")
	FOnTCFOrderTargetingConfirmed OnOrderTargetingConfirmed;

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Order Targeting")
	TSoftObjectPtr<UMaterialInterface> DefaultDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Order Targeting")
	bool bShowInvalidLocationDecal = true;

private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> SelectionComponent;

	UPROPERTY()
	TObjectPtr<UTCFPlayerOrderComponent> PlayerOrderComponent;

	UPROPERTY()
	TObjectPtr<UTCFRTSHoverContextComponent> HoverContextComponent;

	UPROPERTY()
	TObjectPtr<UTCFOrderDefinition> PendingOrderDefinition;

	UPROPERTY()
	TObjectPtr<UDecalComponent> TargetingDecalComponent;

	bool bCurrentTargetValid = false;

	void RefreshTargetingPreview();
	void RefreshCursorOverride() const;
	void RefreshDecal();

	bool SubmitSelfOrder(const UTCFOrderDefinition& OrderDefinition) const;
	bool BuildTargetFromHover(FTCFOrderTarget& OutTarget) const;
	bool IsHoverValidForPendingOrder() const;

	bool IsActorTargetValid() const;
	bool IsLocationTargetValid() const;
	bool IsDirectionTargetValid() const;
	bool IsAreaTargetValid() const;

	void CreateOrUpdateDecal();
	void HideDecal() const;
	void DestroyDecal();

	FVector GetDecalSizeForPendingOrder() const;
	UMaterialInterface* ResolveDecalMaterial() const;
};