//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Player/TCFPlayerController.h"

#include "Components/TCFPlayerMovementCommandComponent.h"
#include "Components/TCFPlayerOrderComponent.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Components/TCFRTSSelectionBoxComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/HitResult.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Components/TCFRTSHoverContextComponent.h"
#include "Player/TCFRTSCameraPawn.h"

ATCFPlayerController::ATCFPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	PlayerSelectionComponent = CreateDefaultSubobject<UTCFPlayerSelectionComponent>(TEXT("PlayerSelectionComponent"));
	RTSSelectionBoxComponent = CreateDefaultSubobject<UTCFRTSSelectionBoxComponent>(TEXT("RTSSelectionBoxComponent"));
	PlayerMovementCommandComponent = CreateDefaultSubobject<UTCFPlayerMovementCommandComponent>(TEXT("PlayerMovementCommandComponent"));
	PlayerOrderComponent = CreateDefaultSubobject<UTCFPlayerOrderComponent>(TEXT("PlayerOrderComponent"));
	RTSHoverContextComponent = CreateDefaultSubobject<UTCFRTSHoverContextComponent>(TEXT("RTSHoverContextComponent"));
}

UTCFPlayerSelectionComponent* ATCFPlayerController::GetPlayerSelectionComponent() const
{
	return PlayerSelectionComponent;
}

UTCFRTSSelectionBoxComponent* ATCFPlayerController::GetRTSSelectionBoxComponent() const
{
	return RTSSelectionBoxComponent;
}

UTCFPlayerMovementCommandComponent* ATCFPlayerController::GetPlayerMovementCommandComponent() const
{
	return PlayerMovementCommandComponent;
}

UTCFPlayerOrderComponent* ATCFPlayerController::GetPlayerOrderComponent() const
{
	return PlayerOrderComponent;
}

UTCFRTSHoverContextComponent* ATCFPlayerController::GetRTSHoverContextComponent() const
{
	return RTSHoverContextComponent;
}

void ATCFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	AddRTSInputMappingContext();
}

void ATCFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}

	if (SelectAction)
	{
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &ATCFPlayerController::HandleSelectStarted);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Completed, this, &ATCFPlayerController::HandleSelectCompleted);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Canceled, this, &ATCFPlayerController::HandleSelectCompleted);
	}

	if (CommandAction)
	{
		EnhancedInputComponent->BindAction(CommandAction, ETriggerEvent::Started, this, &ATCFPlayerController::HandleCommandStarted);
	}

	if (AppendSelectionAction)
	{
		EnhancedInputComponent->BindAction(AppendSelectionAction, ETriggerEvent::Started, this, &ATCFPlayerController::HandleAppendSelectionStarted);
		EnhancedInputComponent->BindAction(AppendSelectionAction, ETriggerEvent::Triggered, this, &ATCFPlayerController::HandleAppendSelectionTriggered);
		EnhancedInputComponent->BindAction(AppendSelectionAction, ETriggerEvent::Completed, this, &ATCFPlayerController::HandleAppendSelectionCompleted);
		EnhancedInputComponent->BindAction(AppendSelectionAction, ETriggerEvent::Canceled, this, &ATCFPlayerController::HandleAppendSelectionCompleted);
	}

	if (CameraPanAction)
	{
		EnhancedInputComponent->BindAction(CameraPanAction, ETriggerEvent::Triggered, this, &ATCFPlayerController::HandleCameraPanTriggered);
		EnhancedInputComponent->BindAction(CameraPanAction, ETriggerEvent::Completed, this, &ATCFPlayerController::HandleCameraPanCompleted);
		EnhancedInputComponent->BindAction(CameraPanAction, ETriggerEvent::Canceled, this, &ATCFPlayerController::HandleCameraPanCompleted);
	}

	if (CameraZoomAction)
	{
		EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &ATCFPlayerController::HandleCameraZoomTriggered);
	}
}

void ATCFPlayerController::AddRTSInputMappingContext() const
{
	if (!RTSInputMappingContext)
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem)
	{
		return;
	}

	InputSubsystem->AddMappingContext(RTSInputMappingContext, RTSInputMappingPriority);
}

bool ATCFPlayerController::TryGetRTSCameraPawn()
{
	if (!CameraPawn)
	{
		CameraPawn = Cast<ATCFRTSCameraPawn>(GetPawn());
		return CameraPawn->IsValidLowLevelFast();
	}
	
	return true;
}

void ATCFPlayerController::HandleSelectStarted(const FInputActionValue& Value)
{
	if (RTSSelectionBoxComponent)
	{
		RTSSelectionBoxComponent->BeginSelection();
	}
}

void ATCFPlayerController::HandleSelectCompleted(const FInputActionValue& Value)
{
	if (RTSSelectionBoxComponent)
	{
		RTSSelectionBoxComponent->EndSelection(IsAppendSelectionActive());
	}
}

void ATCFPlayerController::HandleCommandStarted(const FInputActionValue& Value)
{
	if (!PlayerMovementCommandComponent)
	{
		return;
	}

	FHitResult HitResult;
	if (!GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		return;
	}

	PlayerMovementCommandComponent->MoveSelectedSquadsToLocation(HitResult.Location);
}

void ATCFPlayerController::HandleAppendSelectionStarted(const FInputActionValue& Value)
{
	bAppendSelectionInputActive = Value.Get<bool>();
}

void ATCFPlayerController::HandleAppendSelectionTriggered(const FInputActionValue& Value)
{
	bAppendSelectionInputActive = Value.Get<bool>();
}

void ATCFPlayerController::HandleAppendSelectionCompleted(const FInputActionValue& Value)
{
	bAppendSelectionInputActive = false;
}

void ATCFPlayerController::HandleCameraPanTriggered(const FInputActionValue& Value)
{
	if (!TryGetRTSCameraPawn())
	{
		return;
	}

	CameraPawn->SetPanInput(Value.Get<FVector2D>());
}

void ATCFPlayerController::HandleCameraPanCompleted(const FInputActionValue& Value)
{
	if (!TryGetRTSCameraPawn())
	{
		return;
	}

	CameraPawn->ClearPanInput();
}

void ATCFPlayerController::HandleCameraZoomTriggered(const FInputActionValue& Value)
{
	if (!TryGetRTSCameraPawn())
	{
		return;
	}

	CameraPawn->AddZoomInput(Value.Get<float>());
}

bool ATCFPlayerController::IsAppendSelectionActive() const
{
	return bAppendSelectionInputActive;
}