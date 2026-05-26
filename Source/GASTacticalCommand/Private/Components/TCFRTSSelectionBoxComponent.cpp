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
	if (!PlayerController)
	{
		return false;
	}

	FVector2D CurrentMousePosition;
	const FVector2D BoxEnd = TryGetMouseScreenPosition(CurrentMousePosition)
		? CurrentMousePosition
		: DragEndScreenPosition;

	const FVector2D MinScreenPosition(
		FMath::Min(DragStartScreenPosition.X, BoxEnd.X),
		FMath::Min(DragStartScreenPosition.Y, BoxEnd.Y));

	const FVector2D MaxScreenPosition(
		FMath::Max(DragStartScreenPosition.X, BoxEnd.X),
		FMath::Max(DragStartScreenPosition.Y, BoxEnd.Y));

	FVector2D SquadScreenPosition;
	if (!PlayerController->ProjectWorldLocationToScreen(Squad.GetActorLocation(), SquadScreenPosition))
	{
		return false;
	}

	return SquadScreenPosition.X >= MinScreenPosition.X
		&& SquadScreenPosition.X <= MaxScreenPosition.X
		&& SquadScreenPosition.Y >= MinScreenPosition.Y
		&& SquadScreenPosition.Y <= MaxScreenPosition.Y;
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

	SelectionBoxWidget->AddToViewport(10);
}
