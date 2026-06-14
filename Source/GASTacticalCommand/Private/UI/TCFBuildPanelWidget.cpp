//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFBuildPanelWidget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/PanelWidget.h"
#include "Components/TCFPlayerResourceBankComponent.h"
#include "Data/TCFCommanderBuildCatalogDefinition.h"
#include "Data/TCFConstructionOptionDefinition.h"
#include "Player/TCFPlayerState.h"
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

		const FTCFBuildOptionViewData ViewData = BuildOptionViewData(ConstructionOption);
		OptionButton->SetBuildOptionViewData(ViewData);
	}

	BP_OnBuildPanelDataChanged();
}

void UTCFBuildPanelWidget::HandleBuildOptionClicked(
	UTCFConstructionOptionDefinition* ConstructionOption)
{
	if (!ConstructionOption || !PlayerUISubsystem)
	{
		return;
	}

	const FTCFBuildOptionViewData ViewData = BuildOptionViewData(ConstructionOption);
	if (!ViewData.bCanRequest)
	{
		return;
	}

	CloseBuildPanel();

	if (PlayerUISubsystem->StartConstructionPlacement(ConstructionOption))
	{
		BP_OnBuildOptionRequested(ConstructionOption);
	}
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

	BindBuildOptionButton(OptionButton);

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

		UnbindBuildOptionButton(OptionButton);
		OptionButton->RemoveFromParent();
	}

	BuildOptionButtons.Reset();
}

void UTCFBuildPanelWidget::BindBuildOptionButton(UTCFBuildOptionButtonWidget* OptionButton)
{
	if (!OptionButton)
	{
		return;
	}

	OptionButton->OnBuildOptionClicked.AddUniqueDynamic(
		this,
		&UTCFBuildPanelWidget::HandleBuildOptionClicked);

	OptionButton->OnTooltipRequested.AddUniqueDynamic(
		this,
		&UTCFBuildPanelWidget::HandleTooltipRequested);

	OptionButton->OnTooltipCleared.AddUniqueDynamic(
		this,
		&UTCFBuildPanelWidget::HandleTooltipCleared);
}

void UTCFBuildPanelWidget::UnbindBuildOptionButton(UTCFBuildOptionButtonWidget* OptionButton)
{
	if (!OptionButton)
	{
		return;
	}

	OptionButton->OnBuildOptionClicked.RemoveDynamic(
		this,
		&UTCFBuildPanelWidget::HandleBuildOptionClicked);

	OptionButton->OnTooltipRequested.RemoveDynamic(
		this,
		&UTCFBuildPanelWidget::HandleTooltipRequested);

	OptionButton->OnTooltipCleared.RemoveDynamic(
		this,
		&UTCFBuildPanelWidget::HandleTooltipCleared);
}

void UTCFBuildPanelWidget::HandleTooltipRequested(
	UTCFTooltipSourceWidget* SourceWidget,
	UTCFTooltipWidget* TCFTooltipWidget)
{
	if (PlayerUISubsystem)
	{
		PlayerUISubsystem->RequestTooltip(SourceWidget, TCFTooltipWidget);
	}
}

void UTCFBuildPanelWidget::HandleTooltipCleared(
	UTCFTooltipSourceWidget* SourceWidget)
{
	if (PlayerUISubsystem)
	{
		PlayerUISubsystem->ClearTooltip(SourceWidget);
	}
}

FTCFBuildOptionViewData UTCFBuildPanelWidget::BuildOptionViewData(
	UTCFConstructionOptionDefinition* ConstructionOption) const
{
	FTCFBuildOptionViewData ViewData;
	ViewData.ConstructionOption = ConstructionOption;

	if (!ConstructionOption)
	{
		ViewData.Availability = ETCFActionAvailability::Unavailable;
		ViewData.DisabledReason = FText::FromString(TEXT("Missing construction option."));
		return ViewData;
	}

	ViewData.DisplayName = ConstructionOption->GetSafeDisplayName();
	ViewData.Description = ConstructionOption->GetDescription();
	ViewData.Icon = ConstructionOption->Icon;
	ViewData.Cost = ConstructionOption->GetEffectiveCost();
	ViewData.RequiredConstructionWork = ConstructionOption->GetRequiredConstructionWork();
	ViewData.RequiredTags = ConstructionOption->RequiredCommanderTags;
	ViewData.BlockedTags = ConstructionOption->BlockedCommanderTags;

	ViewData.Availability = ResolveBuildOptionAvailability(
		ViewData,
		ViewData.DisabledReason);

	ViewData.bCanRequest = ViewData.Availability == ETCFActionAvailability::Available;

	return ViewData;
}

ETCFActionAvailability UTCFBuildPanelWidget::ResolveBuildOptionAvailability(
	const FTCFBuildOptionViewData& ViewData,
	FText& OutDisabledReason) const
{
	if (!ViewData.ConstructionOption)
	{
		OutDisabledReason = FText::FromString(TEXT("Missing construction option."));
		return ETCFActionAvailability::Unavailable;
	}

	if (!PlayerSatisfiesRequiredTags(ViewData.RequiredTags))
	{
		OutDisabledReason = FText::FromString(TEXT("Building not unlocked."));
		return ETCFActionAvailability::Locked;
	}

	if (PlayerHasBlockedTags(ViewData.BlockedTags))
	{
		OutDisabledReason = FText::FromString(TEXT("Blocked by current tech or state."));
		return ETCFActionAvailability::Blocked;
	}

	if (!CanAffordCost(ViewData.Cost))
	{
		OutDisabledReason = FText::FromString(TEXT("Insufficient resources."));
		return ETCFActionAvailability::InsufficientResources;
	}

	OutDisabledReason = FText::GetEmpty();
	return ETCFActionAvailability::Available;
}

ATCFPlayerState* UTCFBuildPanelWidget::ResolvePlayerState() const
{
	const APlayerController* PlayerController = GetOwningPlayer();
	return PlayerController
		? PlayerController->GetPlayerState<ATCFPlayerState>()
		: nullptr;
}

bool UTCFBuildPanelWidget::CanAffordCost(
	const TArray<FTCFResourceAmount>& Cost) const
{
	if (Cost.IsEmpty())
	{
		return true;
	}

	const ATCFPlayerState* PlayerState = ResolvePlayerState();
	if (!PlayerState)
	{
		return false;
	}

	const UTCFPlayerResourceBankComponent* ResourceBank =
		PlayerState->GetPlayerResourceBankComponent();

	return ResourceBank && ResourceBank->CanAffordResources(Cost);
}

bool UTCFBuildPanelWidget::PlayerSatisfiesRequiredTags(
	const FGameplayTagContainer& RequiredTags) const
{
	if (RequiredTags.IsEmpty())
	{
		return true;
	}

	const ATCFPlayerState* PlayerState = ResolvePlayerState();
	if (!PlayerState)
	{
		return false;
	}

	const IAbilitySystemInterface* AbilitySystemInterface =
		Cast<IAbilitySystemInterface>(PlayerState);

	const UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface
		? AbilitySystemInterface->GetAbilitySystemComponent()
		: nullptr;

	return AbilitySystemComponent
		&& AbilitySystemComponent->HasAllMatchingGameplayTags(RequiredTags);
}

bool UTCFBuildPanelWidget::PlayerHasBlockedTags(
	const FGameplayTagContainer& BlockedTags) const
{
	if (BlockedTags.IsEmpty())
	{
		return false;
	}

	const ATCFPlayerState* PlayerState = ResolvePlayerState();
	if (!PlayerState)
	{
		return false;
	}

	const IAbilitySystemInterface* AbilitySystemInterface =
		Cast<IAbilitySystemInterface>(PlayerState);

	const UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface
		? AbilitySystemInterface->GetAbilitySystemComponent()
		: nullptr;

	return AbilitySystemComponent
		&& AbilitySystemComponent->HasAnyMatchingGameplayTags(BlockedTags);
}