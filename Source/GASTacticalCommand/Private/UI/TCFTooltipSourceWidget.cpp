//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFTooltipSourceWidget.h"

#include "UI/TCFTooltipWidget.h"

void UTCFTooltipSourceWidget::NativeOnMouseEnter(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (UTCFTooltipWidget* TooltipWidget = GetTooltipWidgetForSource())
	{
		OnTooltipRequested.Broadcast(this, TooltipWidget);
	}
}

void UTCFTooltipSourceWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnTooltipCleared.Broadcast(this);

	Super::NativeOnMouseLeave(InMouseEvent);
}

void UTCFTooltipSourceWidget::NativeOnReleasedToPool()
{
	OnTooltipCleared.Broadcast(this);

	Super::NativeOnReleasedToPool();
}

UTCFTooltipWidget* UTCFTooltipSourceWidget::GetTooltipWidgetForSource()
{
	return nullptr;
}