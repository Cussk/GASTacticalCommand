//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Player/TCFRTSCameraPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"
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

	UpdateEdgeScrollInput();
	UpdatePan(DeltaSeconds);
	UpdateZoom(DeltaSeconds);
	ApplyCameraBounds();
}

void ATCFRTSCameraPawn::SetPanInput(FVector2D NewPanInput)
{
	ManualPanInput = NewPanInput.GetClampedToMaxSize(1.0f);
}

void ATCFRTSCameraPawn::ClearPanInput()
{
	ManualPanInput = FVector2D::ZeroVector;
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

void ATCFRTSCameraPawn::UpdateEdgeScrollInput()
{
	EdgePanInput = FVector2D::ZeroVector;

	if (!bEnableEdgeScrolling)
	{
		return;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	int32 ViewportSizeX = 0;
	int32 ViewportSizeY = 0;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	if (ViewportSizeX <= 0 || ViewportSizeY <= 0)
	{
		return;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!PlayerController->GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	if (MouseX < 0.0f || MouseY < 0.0f || MouseX > ViewportSizeX || MouseY > ViewportSizeY)
	{
		return;
	}

	const float Margin = FMath::Max(1.0f, EdgeScrollMarginPixels);

	if (MouseX <= Margin)
	{
		EdgePanInput.X = -1.0f + MouseX / Margin;
	}
	else if (MouseX >= static_cast<float>(ViewportSizeX) - Margin)
	{
		EdgePanInput.X = (MouseX - (static_cast<float>(ViewportSizeX) - Margin)) / Margin;
	}

	if (MouseY <= Margin)
	{
		EdgePanInput.Y = 1.0f - MouseY / Margin;
	}
	else if (MouseY >= static_cast<float>(ViewportSizeY) - Margin)
	{
		EdgePanInput.Y = -((MouseY - (static_cast<float>(ViewportSizeY) - Margin)) / Margin);
	}

	EdgePanInput = EdgePanInput.GetClampedToMaxSize(1.0f) * EdgeScrollSpeedMultiplier;
}

void ATCFRTSCameraPawn::UpdatePan(float DeltaSeconds)
{
	if (DeltaSeconds <= 0.0f)
	{
		return;
	}

	const FVector2D CombinedPanInput = (ManualPanInput + EdgePanInput).GetClampedToMaxSize(1.0f);
	if (CombinedPanInput.IsNearlyZero())
	{
		return;
	}

	const FRotator YawRotation(0.0f, GetActorRotation().Yaw, 0.0f);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const float EffectivePanSpeed = GetEffectivePanSpeed();

	const FVector Movement =
		Forward * CombinedPanInput.Y * EffectivePanSpeed * DeltaSeconds
		+ Right * CombinedPanInput.X * EffectivePanSpeed * DeltaSeconds;

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

float ATCFRTSCameraPawn::GetEffectivePanSpeed() const
{
	if (!bScalePanSpeedByZoom)
	{
		return PanSpeed;
	}

	const float ZoomAlpha = GetZoomAlpha();
	const float SpeedMultiplier = FMath::Lerp(
		MinZoomPanSpeedMultiplier,
		MaxZoomPanSpeedMultiplier,
		ZoomAlpha);

	return PanSpeed * SpeedMultiplier;
}

float ATCFRTSCameraPawn::GetZoomAlpha() const
{
	const float CurrentZoomDistance = GetCurrentZoomDistance();

	if (FMath::IsNearlyEqual(MinZoomDistance, MaxZoomDistance))
	{
		return 0.0f;
	}

	return FMath::Clamp(
		(CurrentZoomDistance - MinZoomDistance) / (MaxZoomDistance - MinZoomDistance),
		0.0f,
		1.0f);
}