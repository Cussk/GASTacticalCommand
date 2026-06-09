//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFTooltipWidget.h"

const FText& UTCFTooltipWidget::GetTooltipTitle() const
{
	return TooltipTitle;
}

const FText& UTCFTooltipWidget::GetTooltipSubtitle() const
{
	return TooltipSubtitle;
}

void UTCFTooltipWidget::SetTooltipHeader(const FText& InTitle, const FText& InSubtitle)
{
	TooltipTitle = InTitle;
	TooltipSubtitle = InSubtitle;
}

void UTCFTooltipWidget::ClearTooltipHeader()
{
	TooltipTitle = FText::GetEmpty();
	TooltipSubtitle = FText::GetEmpty();
}

void UTCFTooltipWidget::NotifyTooltipDataChanged()
{
	NativeOnTooltipDataChanged();
	BP_OnTooltipDataChanged();
}

void UTCFTooltipWidget::NativeOnTooltipDataChanged()
{
}