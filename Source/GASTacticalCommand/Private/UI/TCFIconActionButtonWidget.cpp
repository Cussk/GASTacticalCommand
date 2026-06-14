//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFIconActionButtonWidget.h"

#include "Components/Button.h"
#include "UI/TCFActionTooltipWidget.h"

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
		ActionTooltipWidget->SetActionViewData(ViewData);
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

UTCFActionTooltipWidget* UTCFIconActionButtonWidget::GetOrCreateActionTooltipWidget()
{
	if (!ActionTooltipWidgetClass)
	{
		return nullptr;
	}

	if (!ActionTooltipWidget)
	{
		ActionTooltipWidget = CreateWidget<UTCFActionTooltipWidget>(
			GetOwningPlayer(),
			ActionTooltipWidgetClass);
	}

	if (ActionTooltipWidget)
	{
		ActionTooltipWidget->SetActionViewData(ViewData);
	}

	return ActionTooltipWidget;
}

void UTCFIconActionButtonWidget::ClearTooltipData() const
{
	if (ActionTooltipWidget)
	{
		ActionTooltipWidget->ClearActionViewData();
	}
}