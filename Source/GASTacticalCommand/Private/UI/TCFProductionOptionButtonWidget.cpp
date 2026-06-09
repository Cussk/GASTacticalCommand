//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFProductionOptionButtonWidget.h"

#include "UI/TCFProductionOptionTooltipWidget.h"

void UTCFProductionOptionButtonWidget::SetOptionViewData(
	const FTCFProductionOptionViewData& InViewData)
{
	ViewData = InViewData;

	BP_OnOptionViewDataChanged();
}

const FTCFProductionOptionViewData& UTCFProductionOptionButtonWidget::GetOptionViewData() const
{
	return ViewData;
}

UTCFProductionOptionDefinition* UTCFProductionOptionButtonWidget::GetProductionOption() const
{
	return ViewData.ProductionOption;
}

bool UTCFProductionOptionButtonWidget::NotifyProductionOptionClicked()
{
	if (!ViewData.bCanRequest || !ViewData.ProductionOption)
	{
		return false;
	}

	OnProductionOptionButtonClicked.Broadcast(this, ViewData.ProductionOption);
	return true;
}

void UTCFProductionOptionButtonWidget::NativeOnReleasedToPool()
{
	Super::NativeOnReleasedToPool();

	ClearTooltipData();
	ViewData = FTCFProductionOptionViewData();
}

UTCFTooltipWidget* UTCFProductionOptionButtonWidget::GetTooltipWidgetForSource()
{
	return GetOrCreateOptionTooltipWidget();
}

UTCFProductionOptionTooltipWidget* UTCFProductionOptionButtonWidget::GetOrCreateOptionTooltipWidget()
{
	if (!OptionTooltipWidgetClass)
	{
		return nullptr;
	}

	if (!OptionTooltipWidget)
	{
		OptionTooltipWidget = CreateWidget<UTCFProductionOptionTooltipWidget>(
			GetOwningPlayer(),
			OptionTooltipWidgetClass);
	}

	if (OptionTooltipWidget)
	{
		OptionTooltipWidget->SetOptionViewData(ViewData);
	}

	return OptionTooltipWidget;
}

void UTCFProductionOptionButtonWidget::ClearTooltipData() const
{
	if (OptionTooltipWidget)
	{
		OptionTooltipWidget->ClearOptionViewData();
	}
}
