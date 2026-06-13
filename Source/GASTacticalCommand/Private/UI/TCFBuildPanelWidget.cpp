//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFBuildPanelWidget.h"

#include "Components/PanelWidget.h"
#include "Data/TCFCommanderBuildCatalogDefinition.h"
#include "Data/TCFConstructionOptionDefinition.h"
#include "Subsystems/TCFPlayerUISubsystem.h"
#include "UI/TCFBuildOptionButtonWidget.h"

void UTCFBuildPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CloseBuildPanel();
	RefreshBuildOptions();
}

void UTCFBuildPanelWidget::NativeDestruct()
{
	ReleaseBuildOptionButtons();
	PlayerUISubsystem = nullptr;

	Super::NativeDestruct();
}

void UTCFBuildPanelWidget::SetPlayerUISubsystem(
	UTCFPlayerUISubsystem* InPlayerUISubsystem)
{
	PlayerUISubsystem = InPlayerUISubsystem;
}

void UTCFBuildPanelWidget::OpenBuildPanel()
{
	if (bIsBuildPanelOpen)
	{
		return;
	}

	bIsBuildPanelOpen = true;
	RefreshBuildOptions();

	OnBuildPanelVisibilityChanged.Broadcast();
	BP_OnBuildPanelDataChanged();
}

void UTCFBuildPanelWidget::CloseBuildPanel()
{
	if (!bIsBuildPanelOpen)
	{
		return;
	}

	bIsBuildPanelOpen = false;

	OnBuildPanelVisibilityChanged.Broadcast();
	BP_OnBuildPanelDataChanged();
}

void UTCFBuildPanelWidget::ToggleBuildPanel()
{
	if (bIsBuildPanelOpen)
	{
		CloseBuildPanel();
	}
	else
	{
		OpenBuildPanel();
	}
}

bool UTCFBuildPanelWidget::IsBuildPanelOpen() const
{
	return bIsBuildPanelOpen;
}

void UTCFBuildPanelWidget::RefreshBuildOptions()
{
	ReleaseBuildOptionButtons();

	if (!ConstructionCatalog || !BuildOptionContainer || !BuildOptionButtonClass)
	{
		BP_OnBuildPanelDataChanged();
		return;
	}

	TArray<UTCFConstructionOptionDefinition*> ConstructionOptions;
	ConstructionCatalog->GetConstructionOptions(ConstructionOptions);

	for (UTCFConstructionOptionDefinition* ConstructionOption : ConstructionOptions)
	{
		if (!ConstructionOption)
		{
			continue;
		}

		UTCFBuildOptionButtonWidget* OptionButton = CreateBuildOptionButton();
		if (!OptionButton)
		{
			continue;
		}

		OptionButton->SetConstructionOption(ConstructionOption);
	}

	BP_OnBuildPanelDataChanged();
}

void UTCFBuildPanelWidget::HandleBuildOptionClicked(
	UTCFConstructionOptionDefinition* ConstructionOption)
{
	if (!ConstructionOption)
	{
		return;
	}

	CloseBuildPanel();

	BP_OnBuildOptionRequested(ConstructionOption);
}

UTCFBuildOptionButtonWidget* UTCFBuildPanelWidget::CreateBuildOptionButton()
{
	if (!BuildOptionContainer || !BuildOptionButtonClass)
	{
		return nullptr;
	}

	UTCFBuildOptionButtonWidget* OptionButton =
		CreateWidget<UTCFBuildOptionButtonWidget>(
			GetOwningPlayer(),
			BuildOptionButtonClass);

	if (!OptionButton)
	{
		return nullptr;
	}

	OptionButton->OnBuildOptionClicked.AddDynamic(
		this,
		&UTCFBuildPanelWidget::HandleBuildOptionClicked);

	BuildOptionContainer->AddChild(OptionButton);
	BuildOptionButtons.Add(OptionButton);

	return OptionButton;
}

void UTCFBuildPanelWidget::ReleaseBuildOptionButtons()
{
	for (UTCFBuildOptionButtonWidget* OptionButton : BuildOptionButtons)
	{
		if (!OptionButton)
		{
			continue;
		}

		OptionButton->OnBuildOptionClicked.RemoveDynamic(
			this,
			&UTCFBuildPanelWidget::HandleBuildOptionClicked);

		OptionButton->RemoveFromParent();
	}

	BuildOptionButtons.Reset();
}