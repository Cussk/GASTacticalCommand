//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFBuildOptionButtonWidget.h"

#include "Data/TCFConstructionOptionDefinition.h"
#include "UI/TCFBuildOptionTooltipWidget.h"

void UTCFBuildOptionButtonWidget::SetBuildOptionViewData(
	const FTCFBuildOptionViewData& InViewData)
{
	ViewData = InViewData;

	SetVisibility(ViewData.ConstructionOption
		? ESlateVisibility::Visible
		: ESlateVisibility::Collapsed);

	if (BuildOptionTooltipWidget)
	{
		BuildOptionTooltipWidget->SetBuildOptionViewData(ViewData);
	}

	BP_OnAcquiredFromPool();
	BP_OnBuildOptionViewDataChanged();
}

const FTCFBuildOptionViewData& UTCFBuildOptionButtonWidget::GetBuildOptionViewData() const
{
	return ViewData;
}

UTCFConstructionOptionDefinition* UTCFBuildOptionButtonWidget::GetConstructionOption() const
{
	return ViewData.ConstructionOption;
}

void UTCFBuildOptionButtonWidget::NotifyBuildOptionClicked()
{
	if (!ViewData.bCanRequest || !ViewData.ConstructionOption)
	{
		return;
	}

	OnBuildOptionClicked.Broadcast(ViewData.ConstructionOption);
}

void UTCFBuildOptionButtonWidget::NativeOnReleasedToPool()
{
	ClearTooltipData();

	ViewData = FTCFBuildOptionViewData();

	Super::NativeOnReleasedToPool();
}

UTCFTooltipWidget* UTCFBuildOptionButtonWidget::GetTooltipWidgetForSource()
{
	return GetOrCreateBuildOptionTooltipWidget();
}

UTCFBuildOptionTooltipWidget* UTCFBuildOptionButtonWidget::GetOrCreateBuildOptionTooltipWidget()
{
	if (!BuildOptionTooltipWidgetClass)
	{
		return nullptr;
	}

	if (!BuildOptionTooltipWidget)
	{
		BuildOptionTooltipWidget = CreateWidget<UTCFBuildOptionTooltipWidget>(
			GetOwningPlayer(),
			BuildOptionTooltipWidgetClass);
	}

	if (BuildOptionTooltipWidget)
	{
		BuildOptionTooltipWidget->SetBuildOptionViewData(ViewData);
	}

	return BuildOptionTooltipWidget;
}

void UTCFBuildOptionButtonWidget::ClearTooltipData() const
{
	if (BuildOptionTooltipWidget)
	{
		BuildOptionTooltipWidget->ClearBuildOptionViewData();
	}
}