//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TCFPlayerController.generated.h"

class UTCFRTSCommandRouterComponent;
class UTCFRTSHoverContextComponent;
class ATCFRTSCameraPawn;
class UInputAction;
class UInputMappingContext;
class UTCFPlayerMovementCommandComponent;
class UTCFPlayerOrderComponent;
class UTCFPlayerSelectionComponent;
class UTCFRTSSelectionBoxComponent;
struct FInputActionValue;

UCLASS()
class GASTACTICALCOMMAND_API ATCFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATCFPlayerController();

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	UTCFPlayerSelectionComponent* GetPlayerSelectionComponent() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Selection")
	UTCFRTSSelectionBoxComponent* GetRTSSelectionBoxComponent() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Movement")
	UTCFPlayerMovementCommandComponent* GetPlayerMovementCommandComponent() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Orders")
	UTCFPlayerOrderComponent* GetPlayerOrderComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Hover")
	UTCFRTSHoverContextComponent* GetRTSHoverContextComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "TCF|RTS Command")
	UTCFRTSCommandRouterComponent* GetRTSCommandRouterComponent() const;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFPlayerSelectionComponent> PlayerSelectionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFRTSSelectionBoxComponent> RTSSelectionBoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFPlayerMovementCommandComponent> PlayerMovementCommandComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFPlayerOrderComponent> PlayerOrderComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFRTSHoverContextComponent> RTSHoverContextComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFRTSCommandRouterComponent> RTSCommandRouterComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Input")
	TObjectPtr<UInputMappingContext> RTSInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Input")
	int32 RTSInputMappingPriority = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Input")
	TObjectPtr<UInputAction> SelectAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Input")
	TObjectPtr<UInputAction> CommandAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Input")
	TObjectPtr<UInputAction> AppendSelectionAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Input")
	TObjectPtr<UInputAction> CameraPanAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Input")
	TObjectPtr<UInputAction> CameraZoomAction;

private:	
	UPROPERTY()
	ATCFRTSCameraPawn* CameraPawn;
	
	bool bAppendSelectionInputActive = false;

	void AddRTSInputMappingContext() const;
	bool TryGetRTSCameraPawn();

	void HandleSelectStarted(const FInputActionValue& Value);
	void HandleSelectCompleted(const FInputActionValue& Value);

	void HandleCommandStarted(const FInputActionValue& Value);

	void HandleAppendSelectionStarted(const FInputActionValue& Value);
	void HandleAppendSelectionTriggered(const FInputActionValue& Value);
	void HandleAppendSelectionCompleted(const FInputActionValue& Value);

	void HandleCameraPanTriggered(const FInputActionValue& Value);
	void HandleCameraPanCompleted(const FInputActionValue& Value);

	void HandleCameraZoomTriggered(const FInputActionValue& Value);

	bool IsAppendSelectionActive() const;
};