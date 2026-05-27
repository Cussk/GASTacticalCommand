//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/TCFRTSControlTypes.h"
#include "TCFRTSCommandRouterComponent.generated.h"

class UTCFPlayerOrderComponent;
class UTCFPlayerMovementCommandComponent;
class UTCFPlayerSelectionComponent;
class UTCFRTSHoverContextComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTCFRTSCommandIntentResolved, FTCFRTSCommandIntent, CommandIntent);

UCLASS(ClassGroup = (TCF), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMAND_API UTCFRTSCommandRouterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFRTSCommandRouterComponent();

	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Command")
	bool ExecutePrimaryCommand();

	UFUNCTION(BlueprintPure, Category = "TCF|RTS Command")
	const FTCFRTSCommandIntent& GetLastCommandIntent() const;

	UPROPERTY(BlueprintAssignable, Category = "TCF|RTS Command")
	FOnTCFRTSCommandIntentResolved OnCommandIntentResolved;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Command")
	FGameplayTag BasicAttackOrderTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Command")
	bool bMoveToCapturePointOnCommand = true;

private:
	UPROPERTY()
	TObjectPtr<UTCFPlayerSelectionComponent> SelectionComponent;

	UPROPERTY()
	TObjectPtr<UTCFPlayerMovementCommandComponent> MovementCommandComponent;

	UPROPERTY()
	TObjectPtr<UTCFRTSHoverContextComponent> HoverContextComponent;
	
	UPROPERTY()
	TObjectPtr<UTCFPlayerOrderComponent> PlayerOrderComponent;

	FTCFRTSCommandIntent LastCommandIntent;

	FTCFRTSCommandIntent BuildCommandIntent() const;
	bool ExecuteCommandIntent(FTCFRTSCommandIntent& CommandIntent) const;
	bool ExecuteAttackTargetIntent(const FTCFRTSCommandIntent& CommandIntent) const;

	ETCFRTSCommandIntentType ResolveIntentType(const FTCFRTSHoverContext& HoverContext) const;
	FVector ResolveTargetLocation(const FTCFRTSHoverContext& HoverContext) const;

	int32 GetSelectedSquadCount() const;
	bool HasSelectedSquads() const;
};