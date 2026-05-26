//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Player/TCFRTSCameraPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"

ATCFRTSCameraPawn::ATCFRTSCameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(SceneRoot);
	SpringArmComponent->TargetArmLength = 1800.0f;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
}

void ATCFRTSCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	if (SpringArmComponent)
	{
		TargetZoomDistance = FMath::Clamp(
			SpringArmComponent->TargetArmLength,
			MinZoomDistance,
			MaxZoomDistance);
	}
}

void ATCFRTSCameraPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdatePan(DeltaSeconds);
	UpdateZoom(DeltaSeconds);
	ApplyCameraBounds();
}

void ATCFRTSCameraPawn::SetPanInput(FVector2D NewPanInput)
{
	PanInput = NewPanInput.GetClampedToMaxSize(1.0f);
}

void ATCFRTSCameraPawn::ClearPanInput()
{
	PanInput = FVector2D::ZeroVector;
}

void ATCFRTSCameraPawn::AddZoomInput(float ZoomInput)
{
	if (FMath::IsNearlyZero(ZoomInput))
	{
		return;
	}

	TargetZoomDistance = FMath::Clamp(
		TargetZoomDistance - ZoomInput * ZoomStep,
		MinZoomDistance,
		MaxZoomDistance);
}

float ATCFRTSCameraPawn::GetCurrentZoomDistance() const
{
	return SpringArmComponent ? SpringArmComponent->TargetArmLength : 0.0f;
}

void ATCFRTSCameraPawn::UpdatePan(float DeltaSeconds)
{
	if (PanInput.IsNearlyZero() || DeltaSeconds <= 0.0f)
	{
		return;
	}

	const FRotator YawRotation(0.0f, GetActorRotation().Yaw, 0.0f);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const FVector Movement =
		Forward * PanInput.Y * PanSpeed * DeltaSeconds
		+ Right * PanInput.X * PanSpeed * DeltaSeconds;

	AddActorWorldOffset(Movement, false);
}

void ATCFRTSCameraPawn::UpdateZoom(const float DeltaSeconds) const
{
	if (!SpringArmComponent || DeltaSeconds <= 0.0f)
	{
		return;
	}

	SpringArmComponent->TargetArmLength = FMath::FInterpTo(
		SpringArmComponent->TargetArmLength,
		TargetZoomDistance,
		DeltaSeconds,
		ZoomInterpSpeed);
}

void ATCFRTSCameraPawn::ApplyCameraBounds()
{
	if (!bClampCameraBounds)
	{
		return;
	}

	FVector Location = GetActorLocation();
	Location.X = FMath::Clamp(Location.X, MinCameraBounds.X, MaxCameraBounds.X);
	Location.Y = FMath::Clamp(Location.Y, MinCameraBounds.Y, MaxCameraBounds.Y);
	SetActorLocation(Location, false);
}