//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFRTSHUDWidget.h"

#include "Components/Widget.h"
#include "UI/TCFProductionPanelWidget.h"

void UTCFRTSHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ProductionPanel)
	{
		ProductionPanel->OnProductionPanelDataChanged.AddDynamic(
			this,
			&UTCFRTSHUDWidget::HandleProductionPanelDataChanged);
	}

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
				? ESlateVisibility::SelfHitTestInvisible
				: ESlateVisibility::Collapsed);
	}
	else if (ProductionPanel)
	{
		ProductionPanel->SetVisibility(
			bShowProductionPanel
				? ESlateVisibility::SelfHitTestInvisible
				: ESlateVisibility::Collapsed);
	}
}

void UTCFRTSHUDWidget::HandleProductionPanelDataChanged()
{
	RefreshHUDPanelVisibility();
}