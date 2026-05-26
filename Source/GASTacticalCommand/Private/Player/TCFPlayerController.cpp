//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Player/TCFPlayerController.h"

#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Engine/HitResult.h"
#include "InputCoreTypes.h"
#include "Components/TCFPlayerMovementCommandComponent.h"
#include "Components/TCFPlayerOrderComponent.h"
#include "Components/TCFRTSSelectionBoxComponent.h"

ATCFPlayerController::ATCFPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	PlayerSelectionComponent = CreateDefaultSubobject<UTCFPlayerSelectionComponent>(TEXT("PlayerSelectionComponent"));
	PlayerMovementCommandComponent = CreateDefaultSubobject<UTCFPlayerMovementCommandComponent>(TEXT("PlayerMovementCommandComponent"));
	PlayerOrderComponent = CreateDefaultSubobject<UTCFPlayerOrderComponent>(TEXT("PlayerOrderComponent"));
	RTSSelectionBoxComponent = CreateDefaultSubobject<UTCFRTSSelectionBoxComponent>(TEXT("RTSSelectionBoxComponent"));
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

void ATCFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());
}

void ATCFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent)
	{
		return;
	}

	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ATCFPlayerController::HandleSelectPressed);
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ATCFPlayerController::HandleSelectReleased);
	InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ATCFPlayerController::HandleMovePressed);
}

void ATCFPlayerController::HandleSelectPressed()
{
	if (RTSSelectionBoxComponent)
	{
		RTSSelectionBoxComponent->BeginSelection();
	}
}

void ATCFPlayerController::HandleSelectReleased()
{
	if (RTSSelectionBoxComponent)
	{
		RTSSelectionBoxComponent->EndSelection(IsAppendSelectionModifierDown());
	}
}

void ATCFPlayerController::HandleMovePressed()
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

bool ATCFPlayerController::IsAppendSelectionModifierDown() const
{
	return IsInputKeyDown(EKeys::LeftShift)
		|| IsInputKeyDown(EKeys::RightShift)
		|| IsInputKeyDown(EKeys::LeftControl)
		|| IsInputKeyDown(EKeys::RightControl);
}
