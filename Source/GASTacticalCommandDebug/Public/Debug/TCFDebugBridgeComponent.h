//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCFDebugBridgeComponent.generated.h"

class UTCFDebugHUDWidget;

UCLASS(ClassGroup = (TCFDebug), meta = (BlueprintSpawnableComponent))
class GASTACTICALCOMMANDDEBUG_API UTCFDebugBridgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTCFDebugBridgeComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Debug")
	TSubclassOf<UTCFDebugHUDWidget> DebugWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Debug")
	bool bCreateWidgetOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Debug", meta = (ClampMin = "0.05"))
	float RefreshInterval = 0.20f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
