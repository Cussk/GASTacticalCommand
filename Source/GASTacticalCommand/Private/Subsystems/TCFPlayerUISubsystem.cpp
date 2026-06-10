//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Subsystems/TCFPlayerUISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Data/TCFResourceUIDefinition.h"
#include "GameFramework/PlayerController.h"
#include "UI/TCFRTSHUDWidget.h"

void UTCFPlayerUISubsystem::Deinitialize()
{
	DestroyRTSHUD();

	ResourceUIDefinition = nullptr;

	Super::Deinitialize();
}

UTCFRTSHUDWidget* UTCFPlayerUISubsystem::CreateRTSHUD(
	APlayerController* OwningPlayerController,
	TSubclassOf<UTCFRTSHUDWidget> RTSHUDWidgetClass)
{
	if (RTSHUDWidget)
	{
		return RTSHUDWidget;
	}

	if (!OwningPlayerController || !RTSHUDWidgetClass)
	{
		return nullptr;
	}

	UTCFRTSHUDWidget* NewHUD = CreateWidget<UTCFRTSHUDWidget>(
		OwningPlayerController,
		RTSHUDWidgetClass);

	if (!NewHUD)
	{
		return nullptr;
	}

	RegisterRTSHUD(NewHUD);
	NewHUD->AddToViewport(100);

	return RTSHUDWidget;
}

void UTCFPlayerUISubsystem::RegisterRTSHUD(UTCFRTSHUDWidget* InRTSHUDWidget)
{
	if (RTSHUDWidget == InRTSHUDWidget)
	{
		return;
	}

	if (RTSHUDWidget)
	{
		RTSHUDWidget->RemoveFromParent();
	}

	RTSHUDWidget = InRTSHUDWidget;

	if (RTSHUDWidget)
	{
		RTSHUDWidget->SetPlayerUISubsystem(this);
	}
}

void UTCFPlayerUISubsystem::DestroyRTSHUD()
{
	if (RTSHUDWidget)
	{
		RTSHUDWidget->RemoveFromParent();
		RTSHUDWidget = nullptr;
	}
}

UTCFRTSHUDWidget* UTCFPlayerUISubsystem::GetRTSHUD() const
{
	return RTSHUDWidget;
}

UTCFProductionPanelWidget* UTCFPlayerUISubsystem::GetProductionPanel() const
{
	return RTSHUDWidget ? RTSHUDWidget->GetProductionPanel() : nullptr;
}

UTCFWorkerPanelWidget* UTCFPlayerUISubsystem::GetWorkerPanel() const
{
	return RTSHUDWidget ? RTSHUDWidget->GetWorkerPanel(): nullptr;
}

bool UTCFPlayerUISubsystem::HasRTSHUD() const
{
	return RTSHUDWidget != nullptr;
}

void UTCFPlayerUISubsystem::SetResourceUIDefinition(
	UTCFResourceUIDefinition* InResourceUIDefinition)
{
	ResourceUIDefinition = InResourceUIDefinition;
}

UTCFResourceUIDefinition* UTCFPlayerUISubsystem::GetResourceUIDefinition() const
{
	return ResourceUIDefinition;
}

void UTCFPlayerUISubsystem::RequestTooltip(
	UTCFTooltipSourceWidget* SourceWidget,
	UTCFTooltipWidget* TooltipWidget)
{
	if (RTSHUDWidget)
	{
		RTSHUDWidget->RequestTooltip(SourceWidget, TooltipWidget);
	}
}

void UTCFPlayerUISubsystem::ClearTooltip(UTCFTooltipSourceWidget* SourceWidget)
{
	if (RTSHUDWidget)
	{
		RTSHUDWidget->ClearTooltip(SourceWidget);
	}
}

void UTCFPlayerUISubsystem::RegisterGameplayInputBlocker(UWidget* Widget)
{
	if (!Widget)
	{
		return;
	}

	GameplayInputBlockers.AddUnique(Widget);
}

void UTCFPlayerUISubsystem::UnregisterGameplayInputBlocker(UWidget* Widget)
{
	GameplayInputBlockers.Remove(Widget);
}

bool UTCFPlayerUISubsystem::IsCursorOverBlockingUI() const
{
	const UTCFRTSHUDWidget* HUD = GetRTSHUD();
	const APlayerController* PlayerController = HUD ? HUD->GetOwningPlayer() : nullptr;

	if (!PlayerController)
	{
		return false;
	}

	const FVector2D SlateCursorPosition = FSlateApplication::Get().GetCursorPos();

	for (const UWidget* Widget : GameplayInputBlockers)
	{
		if (!IsValid(Widget) || Widget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			continue;
		}

		if (IsScreenPositionOverWidget(Widget, SlateCursorPosition))
		{
			return true;
		}
	}

	return false;
}

bool UTCFPlayerUISubsystem::IsScreenPositionOverWidget(const UWidget* Widget, const FVector2D& ScreenPosition) const
{
	if (!IsValid(Widget))
	{
		return false;
	}

	const ESlateVisibility Visibility = Widget->GetVisibility();
	if (Visibility == ESlateVisibility::Collapsed || Visibility == ESlateVisibility::Hidden)
	{
		return false;
	}

	return Widget->GetCachedGeometry().IsUnderLocation(ScreenPosition);
}