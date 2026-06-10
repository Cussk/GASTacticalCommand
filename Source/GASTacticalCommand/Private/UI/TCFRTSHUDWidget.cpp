//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFRTSHUDWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "UI/TCFProductionPanelWidget.h"
#include "UI/TCFResourcePanelWidget.h"
#include "UI/TCFTooltipWidget.h"

void UTCFRTSHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InitializeChildPanels();
	RefreshHUDPanelVisibility();
}

void UTCFRTSHUDWidget::NativeDestruct()
{
	if (ProductionPanel)
	{
		ProductionPanel->OnProductionPanelDataChanged.RemoveDynamic(
			this,
			&UTCFRTSHUDWidget::HandleProductionPanelDataChanged);
	}

	Super::NativeDestruct();
}

bool UTCFRTSHUDWidget::IsMouseOverBlockingUI() const
{
	const APlayerController* PlayerController = GetOwningPlayer();
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

	const FVector2D ScreenPosition(MouseX, MouseY);

	return IsScreenPositionOverWidget(ProductionPanelHost, ScreenPosition)
		|| IsScreenPositionOverWidget(ActiveTooltipWidget, ScreenPosition);
}

bool UTCFRTSHUDWidget::IsScreenPositionOverWidget(const UWidget* Widget, const FVector2D& ScreenPosition) const
{
	if (!Widget || !Widget->IsVisible())
	{
		return false;
	}

	return Widget->GetCachedGeometry().IsUnderLocation(ScreenPosition);
}

void UTCFRTSHUDWidget::RequestTooltip(
	UTCFTooltipSourceWidget* SourceWidget,
	UTCFTooltipWidget* TCFTooltipWidget)
{
	if (!SourceWidget || !TCFTooltipWidget || !TooltipLayer)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(TooltipResetTimerHandle);

	const bool bCanShowImmediately =
		ActiveTooltipWidget != nullptr
		|| World->GetTimerManager().IsTimerActive(TooltipResetTimerHandle);

	PendingTooltipSource = SourceWidget;
	PendingTooltipWidget = TCFTooltipWidget;

	if (bCanShowImmediately)
	{
		World->GetTimerManager().ClearTimer(TooltipShowTimerHandle);
		ShowPendingTooltip();
		return;
	}

	World->GetTimerManager().ClearTimer(TooltipShowTimerHandle);
	World->GetTimerManager().SetTimer(
		TooltipShowTimerHandle,
		this,
		&UTCFRTSHUDWidget::ShowPendingTooltip,
		TooltipShowDelaySeconds,
		false);
}

void UTCFRTSHUDWidget::ClearTooltip(UTCFTooltipSourceWidget* SourceWidget)
{
	if (!SourceWidget)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (PendingTooltipSource == SourceWidget)
	{
		World->GetTimerManager().ClearTimer(TooltipShowTimerHandle);
		PendingTooltipSource = nullptr;
		PendingTooltipWidget = nullptr;
	}

	if (ActiveTooltipSource == SourceWidget)
	{
		World->GetTimerManager().ClearTimer(TooltipResetTimerHandle);
		World->GetTimerManager().SetTimer(
			TooltipResetTimerHandle,
			this,
			&UTCFRTSHUDWidget::HideActiveTooltip,
			TooltipResetGraceSeconds,
			false);
	}
}

void UTCFRTSHUDWidget::ShowPendingTooltip()
{
	ShowTooltipNow(PendingTooltipSource, PendingTooltipWidget);

	PendingTooltipSource = nullptr;
	PendingTooltipWidget = nullptr;
}

void UTCFRTSHUDWidget::ShowTooltipNow(
	UTCFTooltipSourceWidget* SourceWidget,
	UTCFTooltipWidget* TCFTooltipWidget)
{
	if (!SourceWidget || !TCFTooltipWidget || !TooltipLayer)
	{
		return;
	}

	if (ActiveTooltipWidget && ActiveTooltipWidget != TCFTooltipWidget)
	{
		ActiveTooltipWidget->RemoveFromParent();
	}

	ActiveTooltipSource = SourceWidget;
	ActiveTooltipWidget = TCFTooltipWidget;

	if (TCFTooltipWidget->GetParent() != TooltipLayer)
	{
		TCFTooltipWidget->RemoveFromParent();

		if (UCanvasPanelSlot* CanvasSlot = TooltipLayer->AddChildToCanvas(TCFTooltipWidget))
		{
			CanvasSlot->SetAutoSize(true);
			CanvasSlot->SetAlignment(FVector2D::ZeroVector);
		}
	}

	TCFTooltipWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	UpdateActiveTooltipPosition();
}

void UTCFRTSHUDWidget::HideActiveTooltip()
{
	if (ActiveTooltipWidget)
	{
		ActiveTooltipWidget->RemoveFromParent();
	}

	ActiveTooltipWidget = nullptr;
	ActiveTooltipSource = nullptr;
}

void UTCFRTSHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ActiveTooltipWidget)
	{
		UpdateActiveTooltipPosition();
	}
}

void UTCFRTSHUDWidget::UpdateActiveTooltipPosition() const
{
	if (!ActiveTooltipWidget || !TooltipLayer)
	{
		return;
	}

	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		return;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!PlayerController->GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	const FVector2D ScreenMousePosition(MouseX, MouseY);
	const FVector2D LocalMousePosition =
		TooltipLayer->GetCachedGeometry().AbsoluteToLocal(ScreenMousePosition);

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ActiveTooltipWidget->Slot))
	{
		CanvasSlot->SetPosition(LocalMousePosition + TooltipCursorOffset);
	}
}

void UTCFRTSHUDWidget::SetPlayerUISubsystem(
	UTCFPlayerUISubsystem* InPlayerUISubsystem)
{
	PlayerUISubsystem = InPlayerUISubsystem;
	
	InitializeChildPanels();
}

UTCFPlayerUISubsystem* UTCFRTSHUDWidget::GetPlayerUISubsystem() const
{
	return PlayerUISubsystem;
}

UTCFProductionPanelWidget* UTCFRTSHUDWidget::GetProductionPanel() const
{
	return ProductionPanel;
}

void UTCFRTSHUDWidget::RefreshHUDPanelVisibility()
{
	const bool bShowProductionPanel =
		ProductionPanel && ProductionPanel->HasObservedProductionBuilding();

	if (ProductionPanelHost)
	{
		ProductionPanelHost->SetVisibility(
			bShowProductionPanel
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	else if (ProductionPanel)
	{
		ProductionPanel->SetVisibility(
			bShowProductionPanel
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
}

void UTCFRTSHUDWidget::HandleProductionPanelDataChanged()
{
	RefreshHUDPanelVisibility();
}

void UTCFRTSHUDWidget::InitializeChildPanels()
{
	if (ResourcePanel)
	{
		ResourcePanel->SetPlayerUISubsystem(PlayerUISubsystem);
	}
	
	if (ProductionPanel)
	{
		ProductionPanel->SetPlayerUISubsystem(PlayerUISubsystem);

		ProductionPanel->OnProductionPanelDataChanged.AddUniqueDynamic(
			this,
			&UTCFRTSHUDWidget::HandleProductionPanelDataChanged);
	}
}

