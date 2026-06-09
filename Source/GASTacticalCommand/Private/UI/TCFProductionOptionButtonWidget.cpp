//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFProductionOptionButtonWidget.h"

#include "UI/TCFProductionOptionTooltipWidget.h"

void UTCFProductionOptionButtonWidget::SetOptionViewData(
	const FTCFProductionOptionViewData& InViewData)
{
	ViewData = InViewData;

	RefreshTooltip();

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

	ClearTooltip();

	ViewData = FTCFProductionOptionViewData();
}

void UTCFProductionOptionButtonWidget::RefreshTooltip()
{
	if (!OptionTooltipWidgetClass)
	{
		ClearTooltip();
		return;
	}

	if (!OptionTooltipWidget)
	{
		OptionTooltipWidget = CreateWidget<UTCFProductionOptionTooltipWidget>(
			GetOwningPlayer(),
			OptionTooltipWidgetClass);
	}

	if (!OptionTooltipWidget)
	{
		ClearTooltip();
		return;
	}

	OptionTooltipWidget->SetOptionViewData(ViewData);
	SetToolTip(OptionTooltipWidget);
}

void UTCFProductionOptionButtonWidget::ClearTooltip()
{
	if (OptionTooltipWidget)
	{
		OptionTooltipWidget->ClearOptionViewData();
	}

	SetToolTip(nullptr);
}