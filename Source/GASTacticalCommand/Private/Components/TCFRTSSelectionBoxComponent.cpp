//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFRTSSelectionBoxComponent.h"

#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "UI/TCFRTSSelectionBoxWidget.h"

UTCFRTSSelectionBoxComponent::UTCFRTSSelectionBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
	
	SelectionBoxWidgetClass = UTCFRTSSelectionBoxWidget::StaticClass();
}

void UTCFRTSSelectionBoxComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());
	SelectionComponent = GetOwner()
		? GetOwner()->FindComponentByClass<UTCFPlayerSelectionComponent>()
		: nullptr;
	
	CreateSelectionBoxWidget();
}

void UTCFRTSSelectionBoxComponent::BeginSelection()
{
	if (!TryGetMouseScreenPosition(DragStartScreenPosition))
	{
		return;
	}

	DragEndScreenPosition = DragStartScreenPosition;
	bSelectionActive = true;
}

void UTCFRTSSelectionBoxComponent::EndSelection(bool bAppendSelection)
{
	if (!bSelectionActive)
	{
		return;
	}

	TryGetMouseScreenPosition(DragEndScreenPosition);

	if (!SelectionComponent)
	{
		CancelSelection();
		return;
	}

	if (WasDragSelection())
	{
		TArray<ATCFSquadActor*> SquadsInBox;
		GetSquadsInsideSelectionBox(SquadsInBox);
		SelectionComponent->SetSelectedSquads(SquadsInBox, bAppendSelection);
	}
	else
	{
		ATCFSquadActor* SquadUnderCursor = GetSquadUnderCursor();

		if (bAppendSelection)
		{
			SelectionComponent->ToggleSquadSelection(SquadUnderCursor);
		}
		else if (SquadUnderCursor)
		{
			SelectionComponent->TrySelectSquad(SquadUnderCursor);
		}
		else
		{
			SelectionComponent->ClearSelection();
		}
	}

	CancelSelection();
}

void UTCFRTSSelectionBoxComponent::CancelSelection()
{
	bSelectionActive = false;
	DragStartScreenPosition = FVector2D::ZeroVector;
	DragEndScreenPosition = FVector2D::ZeroVector;
}

bool UTCFRTSSelectionBoxComponent::IsDraggingSelection() const
{
	if (!bSelectionActive)
	{
		return false;
	}

	FVector2D CurrentMousePosition;
	if (!TryGetMouseScreenPosition(CurrentMousePosition))
	{
		return false;
	}

	return FVector2D::Distance(DragStartScreenPosition, CurrentMousePosition) >= DragThresholdPixels;
}

void UTCFRTSSelectionBoxComponent::GetSelectionRectangle(FVector2D& OutStart, FVector2D& OutEnd) const
{
	OutStart = DragStartScreenPosition;

	FVector2D CurrentMousePosition;
	OutEnd = TryGetMouseScreenPosition(CurrentMousePosition)
		? CurrentMousePosition
		: DragEndScreenPosition;
}

bool UTCFRTSSelectionBoxComponent::TryGetMouseScreenPosition(FVector2D& OutScreenPosition) const
{
	if (!PlayerController)
	{
		return false;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;

	if (!PlayerController->GetMousePosition(MouseX, MouseY))
	{
		return false;
	}

	OutScreenPosition = FVector2D(MouseX, MouseY);
	return true;
}

bool UTCFRTSSelectionBoxComponent::WasDragSelection() const
{
	return FVector2D::Distance(DragStartScreenPosition, DragEndScreenPosition) >= DragThresholdPixels;
}

ATCFSquadActor* UTCFRTSSelectionBoxComponent::GetSquadUnderCursor() const
{
	if (!PlayerController)
	{
		return nullptr;
	}

	FHitResult HitResult;
	if (!PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		return nullptr;
	}

	return Cast<ATCFSquadActor>(HitResult.GetActor());
}

void UTCFRTSSelectionBoxComponent::GetSquadsInsideSelectionBox(TArray<ATCFSquadActor*>& OutSquads) const
{
	OutSquads.Reset();

	UWorld* World = GetWorld();
	if (!World || !PlayerController)
	{
		return;
	}

	for (TActorIterator<ATCFSquadActor> It(World); It; ++It)
	{
		ATCFSquadActor* Squad = *It;
		if (!IsValid(Squad))
		{
			continue;
		}

		if (IsSquadInsideSelectionBox(*Squad))
		{
			OutSquads.Add(Squad);
		}
	}
}

bool UTCFRTSSelectionBoxComponent::IsSquadInsideSelectionBox(const ATCFSquadActor& Squad) const
{
	FVector2D SelectionMin;
	FVector2D SelectionMax;
	if (!GetSelectionScreenBounds(SelectionMin, SelectionMax))
	{
		return false;
	}

	FVector2D SquadMin;
	FVector2D SquadMax;
	if (!GetActorScreenBounds(Squad, SquadMin, SquadMax))
	{
		return false;
	}

	return DoScreenRectanglesOverlap(
		SelectionMin,
		SelectionMax,
		SquadMin,
		SquadMax);
}

bool UTCFRTSSelectionBoxComponent::GetSelectionScreenBounds(
	FVector2D& OutMinScreenPosition,
	FVector2D& OutMaxScreenPosition) const
{
	FVector2D CurrentMousePosition;
	const FVector2D BoxEnd = TryGetMouseScreenPosition(CurrentMousePosition)
		? CurrentMousePosition
		: DragEndScreenPosition;

	OutMinScreenPosition = FVector2D(
		FMath::Min(DragStartScreenPosition.X, BoxEnd.X),
		FMath::Min(DragStartScreenPosition.Y, BoxEnd.Y));

	OutMaxScreenPosition = FVector2D(
		FMath::Max(DragStartScreenPosition.X, BoxEnd.X),
		FMath::Max(DragStartScreenPosition.Y, BoxEnd.Y));

	return OutMaxScreenPosition.X > OutMinScreenPosition.X
		&& OutMaxScreenPosition.Y > OutMinScreenPosition.Y;
}

bool UTCFRTSSelectionBoxComponent::GetActorScreenBounds(
	const AActor& Actor,
	FVector2D& OutMinScreenPosition,
	FVector2D& OutMaxScreenPosition) const
{
	if (!PlayerController)
	{
		return false;
	}

	FVector Origin;
	FVector BoxExtent;
	Actor.GetActorBounds(false, Origin, BoxExtent);

	const FVector Corners[8] =
	{
		Origin + FVector( BoxExtent.X,  BoxExtent.Y,  BoxExtent.Z),
		Origin + FVector( BoxExtent.X,  BoxExtent.Y, -BoxExtent.Z),
		Origin + FVector( BoxExtent.X, -BoxExtent.Y,  BoxExtent.Z),
		Origin + FVector( BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z),
		Origin + FVector(-BoxExtent.X,  BoxExtent.Y,  BoxExtent.Z),
		Origin + FVector(-BoxExtent.X,  BoxExtent.Y, -BoxExtent.Z),
		Origin + FVector(-BoxExtent.X, -BoxExtent.Y,  BoxExtent.Z),
		Origin + FVector(-BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z)
	};

	bool bProjectedAnyCorner = false;

	OutMinScreenPosition = FVector2D(TNumericLimits<float>::Max(), TNumericLimits<float>::Max());
	OutMaxScreenPosition = FVector2D(TNumericLimits<float>::Lowest(), TNumericLimits<float>::Lowest());

	for (const FVector& Corner : Corners)
	{
		FVector2D ScreenPosition;
		if (!PlayerController->ProjectWorldLocationToScreen(Corner, ScreenPosition))
		{
			continue;
		}

		bProjectedAnyCorner = true;

		OutMinScreenPosition.X = FMath::Min(OutMinScreenPosition.X, ScreenPosition.X);
		OutMinScreenPosition.Y = FMath::Min(OutMinScreenPosition.Y, ScreenPosition.Y);

		OutMaxScreenPosition.X = FMath::Max(OutMaxScreenPosition.X, ScreenPosition.X);
		OutMaxScreenPosition.Y = FMath::Max(OutMaxScreenPosition.Y, ScreenPosition.Y);
	}

	return bProjectedAnyCorner
		&& OutMaxScreenPosition.X > OutMinScreenPosition.X
		&& OutMaxScreenPosition.Y > OutMinScreenPosition.Y;
}

bool UTCFRTSSelectionBoxComponent::DoScreenRectanglesOverlap(
	const FVector2D& AMin,
	const FVector2D& AMax,
	const FVector2D& BMin,
	const FVector2D& BMax)
{
	return AMin.X <= BMax.X
		&& AMax.X >= BMin.X
		&& AMin.Y <= BMax.Y
		&& AMax.Y >= BMin.Y;
}

void UTCFRTSSelectionBoxComponent::CreateSelectionBoxWidget()
{
	if (SelectionBoxWidget || !SelectionBoxWidgetClass)
	{
		return;
	}

	APlayerController* OwningPlayerController = Cast<APlayerController>(GetOwner());
	SelectionBoxWidget = CreateWidget<UTCFRTSSelectionBoxWidget>( OwningPlayerController, SelectionBoxWidgetClass);
	if (!SelectionBoxWidget)
	{
		return;
	}

	SelectionBoxWidget->InitializeSelectionBoxComponent(this);
	SelectionBoxWidget->AddToViewport(10);
}
