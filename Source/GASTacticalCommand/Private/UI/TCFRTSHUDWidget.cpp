//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFRTSHUDWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"
#include "Subsystems/TCFPlayerUISubsystem.h"
#include "UI/TCFBuildPanelWidget.h"
#include "UI/TCFProductionPanelWidget.h"
#include "UI/TCFResourcePanelWidget.h"
#include "UI/TCFTooltipWidget.h"
#include "UI/TCFWorkerPanelWidget.h"

void UTCFRTSHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InitializeChildPanels();
	RefreshHUDPanelVisibility();
}

void UTCFRTSHUDWidget::NativeDestruct()
{
	UninitializeChildPanels();

	Super::NativeDestruct();
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
	
	TCFTooltipWidget->SetRenderOpacity(0.0f);

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
	
	TCFTooltipWidget->ForceLayoutPrepass();
	UpdateActiveTooltipPosition();
	
	TCFTooltipWidget->SetRenderOpacity(1.0f);
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
	RegisterGameplayInputBlockers();
}

UTCFPlayerUISubsystem* UTCFRTSHUDWidget::GetPlayerUISubsystem() const
{
	return PlayerUISubsystem;
}

UTCFProductionPanelWidget* UTCFRTSHUDWidget::GetProductionPanel() const
{
	return ProductionPanel;
}

UTCFWorkerPanelWidget* UTCFRTSHUDWidget::GetWorkerPanel() const
{
	return WorkerPanel;
}

UTCFBuildPanelWidget* UTCFRTSHUDWidget::GetBuildPanel() const
{
	return BuildPanel;
}

void UTCFRTSHUDWidget::RefreshHUDPanelVisibility()
{
	SetPanelVisibility(ResourcePanel, ResourcePanel != nullptr);
	SetPanelVisibility(ProductionPanel,ProductionPanel && ProductionPanel->HasObservedProductionBuilding());
	SetPanelVisibility(WorkerPanel,WorkerPanel && WorkerPanel->HasSelectedWorkers());
	SetPanelVisibility(BuildPanel,BuildPanel && BuildPanel->IsBuildPanelOpen());
}

void UTCFRTSHUDWidget::HandleProductionPanelDataChanged()
{
	RefreshHUDPanelVisibility();
}

void UTCFRTSHUDWidget::HandleWorkerPanelDataChanged()
{
	RefreshHUDPanelVisibility();
}

void UTCFRTSHUDWidget::HandleBuildPanelVisibilityChanged()
{
	RefreshHUDPanelVisibility();
}

void UTCFRTSHUDWidget::SetPanelVisibility(UWidget* PanelWidget, const bool bShouldShow) const
{
	if (!PanelWidget)
	{
		return;
	}

	PanelWidget->SetVisibility(bShouldShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
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
	
	if (WorkerPanel)
	{
		WorkerPanel->SetPlayerUISubsystem(PlayerUISubsystem);
		
		WorkerPanel->OnWorkerPanelDataChanged.AddUniqueDynamic(
			this,
			&UTCFRTSHUDWidget::HandleWorkerPanelDataChanged);
	}
	
	if (BuildPanel)
	{
		BuildPanel->SetPlayerUISubsystem(PlayerUISubsystem);
		
		BuildPanel->OnBuildPanelVisibilityChanged.AddUniqueDynamic(
			this,
			&UTCFRTSHUDWidget::HandleBuildPanelVisibilityChanged);
	}
}

void UTCFRTSHUDWidget::UninitializeChildPanels()
{
	if (ResourcePanel)
	{
		PlayerUISubsystem->UnregisterGameplayInputBlocker(ResourcePanel);
	}
	
	if (ProductionPanel)
	{
		ProductionPanel->OnProductionPanelDataChanged.RemoveDynamic(
			this,
			&UTCFRTSHUDWidget::HandleProductionPanelDataChanged);
		
		PlayerUISubsystem->UnregisterGameplayInputBlocker(ProductionPanel);
	}

	if (WorkerPanel)
	{		
		WorkerPanel->OnWorkerPanelDataChanged.RemoveDynamic(
			this,
			&UTCFRTSHUDWidget::HandleWorkerPanelDataChanged);
		
		PlayerUISubsystem->UnregisterGameplayInputBlocker(WorkerPanel);
	}

	if (BuildPanel)
	{
		BuildPanel->OnBuildPanelVisibilityChanged.RemoveDynamic(
			this,
			&UTCFRTSHUDWidget::HandleBuildPanelVisibilityChanged);
		
		PlayerUISubsystem->UnregisterGameplayInputBlocker(BuildPanel);
	}
}

void UTCFRTSHUDWidget::RegisterGameplayInputBlockers() const
{
	if (ProductionPanel)
	{
		PlayerUISubsystem->RegisterGameplayInputBlocker(ProductionPanel);
	}

	if (ResourcePanel)
	{
		PlayerUISubsystem->RegisterGameplayInputBlocker(ResourcePanel);
	}

	if (WorkerPanel)
	{
		PlayerUISubsystem->RegisterGameplayInputBlocker(WorkerPanel);
	}
	
	if (BuildPanel)
	{
		PlayerUISubsystem->RegisterGameplayInputBlocker(BuildPanel);
	}
}

