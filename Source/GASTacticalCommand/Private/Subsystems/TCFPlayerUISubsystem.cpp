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

bool UTCFPlayerUISubsystem::IsCursorOverBlockingUI() const
{
	return RTSHUDWidget && RTSHUDWidget->IsMouseOverBlockingUI();
}