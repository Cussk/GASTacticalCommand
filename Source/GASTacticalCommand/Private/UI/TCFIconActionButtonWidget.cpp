//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFIconActionButtonWidget.h"

#include "Components/Button.h"
#include "UI/TCFStandardTooltipWidget.h"

void UTCFIconActionButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ActionButton)
	{
		ActionButton->OnClicked.AddUniqueDynamic(
			this,
			&UTCFIconActionButtonWidget::HandleButtonClicked);
	}
}

void UTCFIconActionButtonWidget::NativeOnReleasedToPool()
{
	ClearTooltipData();

	ViewData = FTCFActionButtonViewData();

	Super::NativeOnReleasedToPool();
}

void UTCFIconActionButtonWidget::SetActionViewData(
	const FTCFActionButtonViewData& InViewData)
{
	ViewData = InViewData;

	if (ActionTooltipWidget)
	{
		ActionTooltipWidget->SetTooltipViewData(BuildTooltipViewData());
	}

	BP_OnActionViewDataChanged();
}

const FTCFActionButtonViewData& UTCFIconActionButtonWidget::GetActionViewData() const
{
	return ViewData;
}

void UTCFIconActionButtonWidget::NotifyActionClicked()
{
	if (!ViewData.bCanExecute)
	{
		return;
	}

	OnActionClicked.Broadcast(ViewData.ActionId);
}

void UTCFIconActionButtonWidget::HandleButtonClicked()
{
	NotifyActionClicked();
}

UTCFTooltipWidget* UTCFIconActionButtonWidget::GetTooltipWidgetForSource()
{
	return GetOrCreateActionTooltipWidget();
}

UTCFStandardTooltipWidget* UTCFIconActionButtonWidget::GetOrCreateActionTooltipWidget()
{
	if (!ActionTooltipWidgetClass)
	{
		return nullptr;
	}

	if (!ActionTooltipWidget)
	{
		ActionTooltipWidget = CreateWidget<UTCFStandardTooltipWidget>(
			GetOwningPlayer(),
			ActionTooltipWidgetClass);
	}

	if (ActionTooltipWidget)
	{
		ActionTooltipWidget->SetTooltipViewData(BuildTooltipViewData());
	}

	return ActionTooltipWidget;
}

FTCFTooltipViewData UTCFIconActionButtonWidget::BuildTooltipViewData() const
{
	FTCFTooltipViewData TooltipData;
	TooltipData.Title = ViewData.DisplayName;
	TooltipData.Description = ViewData.Description;
	TooltipData.Icon = ViewData.Icon;
	TooltipData.DisabledReason = ViewData.DisabledReason;
	TooltipData.Availability = ViewData.Availability;
	TooltipData.bCanExecute = ViewData.bCanExecute;

	return TooltipData;
}

void UTCFIconActionButtonWidget::ClearTooltipData() const
{
	if (ActionTooltipWidget)
	{
		ActionTooltipWidget->ClearTooltipViewData();
	}
}