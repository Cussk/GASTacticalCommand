//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFBuildOptionButtonWidget.h"

#include "UI/TCFStandardTooltipWidget.h"

void UTCFBuildOptionButtonWidget::SetBuildOptionViewData(
	const FTCFBuildOptionViewData& InViewData)
{
	ViewData = InViewData;

	SetVisibility(ViewData.ConstructionOption
		? ESlateVisibility::Visible
		: ESlateVisibility::Collapsed);

	if (BuildOptionTooltipWidget)
	{
		BuildOptionTooltipWidget->SetTooltipViewData(BuildTooltipViewData());
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

UTCFStandardTooltipWidget* UTCFBuildOptionButtonWidget::GetOrCreateBuildOptionTooltipWidget()
{
	if (!BuildOptionTooltipWidgetClass)
	{
		return nullptr;
	}

	if (!BuildOptionTooltipWidget)
	{
		BuildOptionTooltipWidget = CreateWidget<UTCFStandardTooltipWidget>(
			GetOwningPlayer(),
			BuildOptionTooltipWidgetClass);
	}

	if (BuildOptionTooltipWidget)
	{
		BuildOptionTooltipWidget->SetTooltipViewData(BuildTooltipViewData());
	}

	return BuildOptionTooltipWidget;
}

FTCFTooltipViewData UTCFBuildOptionButtonWidget::BuildTooltipViewData() const
{
	FTCFTooltipViewData TooltipData;
	TooltipData.Title = ViewData.DisplayName;
	TooltipData.Description = ViewData.Description;
	TooltipData.Icon = ViewData.Icon;
	TooltipData.Cost = ViewData.Cost;
	TooltipData.DisabledReason = ViewData.DisabledReason;
	TooltipData.Availability = ViewData.Availability;
	TooltipData.bCanExecute = ViewData.bCanRequest;
	TooltipData.RequiredConstructionWork = ViewData.RequiredConstructionWork;
	TooltipData.bHasCost = !ViewData.Cost.IsEmpty();
	TooltipData.bHasConstructionWork = ViewData.RequiredConstructionWork > 0.0f;

	return TooltipData;
}

void UTCFBuildOptionButtonWidget::ClearTooltipData() const
{
	if (BuildOptionTooltipWidget)
	{
		BuildOptionTooltipWidget->ClearTooltipViewData();
	}
}