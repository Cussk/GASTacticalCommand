//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TCFPooledWidget.generated.h"

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFPooledWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Widget Pool")
	void AcquireFromPool();

	UFUNCTION(BlueprintCallable, Category = "TCF|Widget Pool")
	void ReleaseToPool();

	UFUNCTION(BlueprintPure, Category = "TCF|Widget Pool")
	bool IsPoolActive() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Widget Pool")
	void BP_OnAcquiredFromPool();

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Widget Pool")
	void BP_OnReleasedToPool();

	virtual void NativeOnAcquiredFromPool();
	virtual void NativeOnReleasedToPool();

private:
	UPROPERTY(VisibleInstanceOnly, Category = "TCF|Widget Pool")
	bool bPoolActive = false;
};