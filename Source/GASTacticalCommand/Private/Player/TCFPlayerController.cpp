//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Player/TCFPlayerController.h"

#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Engine/HitResult.h"
#include "InputCoreTypes.h"
#include "Components/TCFPlayerMovementCommandComponent.h"
#include "Components/TCFPlayerOrderComponent.h"

ATCFPlayerController::ATCFPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	PlayerSelectionComponent = CreateDefaultSubobject<UTCFPlayerSelectionComponent>(TEXT("PlayerSelectionComponent"));
	PlayerMovementCommandComponent = CreateDefaultSubobject<UTCFPlayerMovementCommandComponent>(TEXT("PlayerMovementCommandComponent"));
	PlayerOrderComponent = CreateDefaultSubobject<UTCFPlayerOrderComponent>(TEXT("PlayerOrderComponent"));
}

UTCFPlayerSelectionComponent* ATCFPlayerController::GetPlayerSelectionComponent() const
{
	return PlayerSelectionComponent;
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
}

void ATCFPlayerController::HandleSelectPressed()
{
	if (!PlayerSelectionComponent)
	{
		return;
	}

	if (ATCFSquadActor* HitSquad = GetSquadUnderCursor())
	{
		PlayerSelectionComponent->TrySelectSquad(HitSquad);
		return;
	}

	PlayerSelectionComponent->ClearSelection();
}

ATCFSquadActor* ATCFPlayerController::GetSquadUnderCursor() const
{
	FHitResult HitResult;
	if (!GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		return nullptr;
	}

	AActor* HitActor = HitResult.GetActor();
	return Cast<ATCFSquadActor>(HitActor);
}