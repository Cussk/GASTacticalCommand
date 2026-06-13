//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFBuildOptionButtonWidget.h"

#include "Data/TCFConstructionOptionDefinition.h"

void UTCFBuildOptionButtonWidget::SetConstructionOption(
	UTCFConstructionOptionDefinition* InConstructionOption)
{
	ConstructionOption = InConstructionOption;
	SetVisibility(ConstructionOption ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	BP_OnConstructionOptionChanged();
}

UTCFConstructionOptionDefinition* UTCFBuildOptionButtonWidget::GetConstructionOption() const
{
	return ConstructionOption;
}

void UTCFBuildOptionButtonWidget::NotifyBuildOptionClicked()
{
	if (ConstructionOption)
	{
		OnBuildOptionClicked.Broadcast(ConstructionOption);
	}
}