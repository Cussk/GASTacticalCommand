//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFWorkerPanelWidget.h"

#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Components/TCFRTSCommandRouterComponent.h"
#include "Player/TCFPlayerController.h"
#include "Subsystems/TCFPlayerUISubsystem.h"
#include "TCFGameplayTags.h"
#include "UI/TCFIconActionButtonWidget.h"

const FName UTCFWorkerPanelWidget::BuildActionId(TEXT("Build"));
const FName UTCFWorkerPanelWidget::StopActionId(TEXT("Stop"));

void UTCFWorkerPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!WorkerRoleTag.IsValid())
	{
		WorkerRoleTag = TCFGameplayTags::Squad_Role_Worker;
	}

	BindSelectionComponent();
	BindActionButton(BuildActionButton);
	BindActionButton(StopActionButton);
	RefreshActionButtons();
	RefreshWorkerPanel();
}

void UTCFWorkerPanelWidget::NativeDestruct()
{
	UnbindSelectionComponent();
	UnbindActionButton(BuildActionButton);
	UnbindActionButton(StopActionButton);

	PlayerUISubsystem = nullptr;

	Super::NativeDestruct();
}

void UTCFWorkerPanelWidget::SetPlayerUISubsystem(UTCFPlayerUISubsystem* InPlayerUISubsystem)
{
	PlayerUISubsystem = InPlayerUISubsystem;

	BindSelectionComponent();
	RefreshWorkerPanel();
}

UTCFPlayerUISubsystem* UTCFWorkerPanelWidget::GetPlayerUISubsystem() const
{
	return PlayerUISubsystem;
}

void UTCFWorkerPanelWidget::RefreshWorkerPanel()
{
	SelectedWorkerCount = 0;
	SelectedNonWorkerCount = 0;

	if (UTCFPlayerSelectionComponent* SelectionComponent = ResolveSelectionComponent())
	{
		TArray<ATCFSquadActor*> SelectedSquads;
		SelectionComponent->GetSelectedSquads(SelectedSquads);

		for (const ATCFSquadActor* SelectedSquad : SelectedSquads)
		{
			if (!IsValid(SelectedSquad))
			{
				continue;
			}

			if (IsWorkerSquad(SelectedSquad))
			{
				SelectedWorkerCount++;
			}
			else
			{
				SelectedNonWorkerCount++;
			}
		}
	}

	bHasSelectedWorkers = SelectedWorkerCount > 0;

	RefreshActionButtons();
	OnWorkerPanelDataChanged.Broadcast();
	BP_OnWorkerPanelDataChanged();
}

void UTCFWorkerPanelWidget::RequestBuildMenu()
{
	if (!bHasSelectedWorkers || !PlayerUISubsystem)
	{
		return;
	}

	PlayerUISubsystem->OpenBuildPanel();
}

void UTCFWorkerPanelWidget::RequestStopCommands()
{
	ATCFPlayerController* PlayerController = ResolveTCFPlayerController();
	if (!PlayerController)
	{
		return;
	}

	UTCFRTSCommandRouterComponent* CommandRouterComponent =
		PlayerController->GetRTSCommandRouterComponent();

	if (!CommandRouterComponent)
	{
		return;
	}

	CommandRouterComponent->StopSelectedCommands();
}

int32 UTCFWorkerPanelWidget::GetSelectedWorkerCount() const
{
	return SelectedWorkerCount;
}

int32 UTCFWorkerPanelWidget::GetSelectedNonWorkerCount() const
{
	return SelectedNonWorkerCount;
}

bool UTCFWorkerPanelWidget::HasSelectedWorkers() const
{
	return bHasSelectedWorkers;
}

bool UTCFWorkerPanelWidget::HasOnlySelectedWorkers() const
{
	return bHasSelectedWorkers && SelectedNonWorkerCount == 0;
}

void UTCFWorkerPanelWidget::HandleSelectionCountChanged(int32 SelectedCount)
{
	RefreshWorkerPanel();
}

void UTCFWorkerPanelWidget::HandlePrimarySelectedSquadChanged(ATCFSquadActor* SelectedSquad)
{
	RefreshWorkerPanel();
}

void UTCFWorkerPanelWidget::BindSelectionComponent()
{
	UTCFPlayerSelectionComponent* SelectionComponent = ResolveSelectionComponent();
	if (!SelectionComponent || ObservedSelectionComponent == SelectionComponent)
	{
		return;
	}

	UnbindSelectionComponent();

	ObservedSelectionComponent = SelectionComponent;

	ObservedSelectionComponent->OnSelectionCountChanged.AddDynamic(
		this,
		&UTCFWorkerPanelWidget::HandleSelectionCountChanged);

	ObservedSelectionComponent->OnSelectedSquadChanged.AddDynamic(
		this,
		&UTCFWorkerPanelWidget::HandlePrimarySelectedSquadChanged);
}

void UTCFWorkerPanelWidget::UnbindSelectionComponent()
{
	if (!ObservedSelectionComponent)
	{
		return;
	}

	ObservedSelectionComponent->OnSelectionCountChanged.RemoveDynamic(
		this,
		&UTCFWorkerPanelWidget::HandleSelectionCountChanged);

	ObservedSelectionComponent->OnSelectedSquadChanged.RemoveDynamic(
		this,
		&UTCFWorkerPanelWidget::HandlePrimarySelectedSquadChanged);

	ObservedSelectionComponent = nullptr;
}

ATCFPlayerController* UTCFWorkerPanelWidget::ResolveTCFPlayerController() const
{
	return Cast<ATCFPlayerController>(GetOwningPlayer());
}

UTCFPlayerSelectionComponent* UTCFWorkerPanelWidget::ResolveSelectionComponent() const
{
	const ATCFPlayerController* PlayerController = ResolveTCFPlayerController();
	return PlayerController
		? PlayerController->GetPlayerSelectionComponent()
		: nullptr;
}

bool UTCFWorkerPanelWidget::IsWorkerSquad(const ATCFSquadActor* Squad) const
{
	if (!IsValid(Squad) || !WorkerRoleTag.IsValid())
	{
		return false;
	}

	const FGameplayTag SquadRoleTag = Squad->GetRoleTag();
	return SquadRoleTag.IsValid() && SquadRoleTag.MatchesTagExact(WorkerRoleTag);
}

void UTCFWorkerPanelWidget::RefreshActionButtons() const
{
	if (BuildActionButton)
	{
		BuildActionButton->SetActionViewData(BuildBuildActionViewData());
	}

	if (StopActionButton)
	{
		StopActionButton->SetActionViewData(BuildStopActionViewData());
	}
}

FTCFActionButtonViewData UTCFWorkerPanelWidget::BuildBuildActionViewData() const
{
	FTCFActionButtonViewData ViewData;
	ViewData.ActionId = BuildActionId;
	ViewData.DisplayName = FText::FromString(TEXT("Build"));
	ViewData.Description = FText::FromString(
		TEXT("Open the construction menu for selected workers."));

	ViewData.bCanExecute = bHasSelectedWorkers;
	ViewData.Availability = ViewData.bCanExecute
		? ETCFActionAvailability::Available
		: ETCFActionAvailability::Unavailable;

	ViewData.DisabledReason = ViewData.bCanExecute
		? FText::GetEmpty()
		: FText::FromString(TEXT("Select at least one worker."));
	
	ViewData.Icon = BuildButtonIcon;

	return ViewData;
}

FTCFActionButtonViewData UTCFWorkerPanelWidget::BuildStopActionViewData() const
{
	FTCFActionButtonViewData ViewData;
	ViewData.ActionId = StopActionId;
	ViewData.DisplayName = FText::FromString(TEXT("Stop"));
	ViewData.Description = FText::FromString(
		TEXT("Stop selected squads' current attack, gather, and build commands."));

	ViewData.bCanExecute = bHasSelectedWorkers || SelectedNonWorkerCount > 0;
	ViewData.Availability = ViewData.bCanExecute
		? ETCFActionAvailability::Available
		: ETCFActionAvailability::Unavailable;

	ViewData.DisabledReason = ViewData.bCanExecute
		? FText::GetEmpty()
		: FText::FromString(TEXT("No selected units."));
	
	ViewData.Icon = StopButtonIcon;

	return ViewData;
}

void UTCFWorkerPanelWidget::BindActionButton(
	UTCFIconActionButtonWidget* ActionButton)
{
	if (!ActionButton)
	{
		return;
	}

	ActionButton->OnActionClicked.AddUniqueDynamic(
		this,
		&UTCFWorkerPanelWidget::HandleWorkerActionClicked);

	ActionButton->OnTooltipRequested.AddUniqueDynamic(
		this,
		&UTCFWorkerPanelWidget::HandleTooltipRequested);

	ActionButton->OnTooltipCleared.AddUniqueDynamic(
		this,
		&UTCFWorkerPanelWidget::HandleTooltipCleared);
}

void UTCFWorkerPanelWidget::UnbindActionButton(
	UTCFIconActionButtonWidget* ActionButton)
{
	if (!ActionButton)
	{
		return;
	}

	ActionButton->OnActionClicked.RemoveDynamic(
		this,
		&UTCFWorkerPanelWidget::HandleWorkerActionClicked);

	ActionButton->OnTooltipRequested.RemoveDynamic(
		this,
		&UTCFWorkerPanelWidget::HandleTooltipRequested);

	ActionButton->OnTooltipCleared.RemoveDynamic(
		this,
		&UTCFWorkerPanelWidget::HandleTooltipCleared);
}

void UTCFWorkerPanelWidget::HandleWorkerActionClicked(FName ActionId)
{
	if (ActionId == BuildActionId)
	{
		RequestBuildMenu();
		return;
	}

	if (ActionId == StopActionId)
	{
		RequestStopCommands();
	}
}

void UTCFWorkerPanelWidget::HandleTooltipRequested(UTCFTooltipSourceWidget* SourceWidget, UTCFTooltipWidget* TCFTooltipWidget)
{
	if (PlayerUISubsystem)
	{
		PlayerUISubsystem->RequestTooltip(SourceWidget, TCFTooltipWidget);
	}
}

void UTCFWorkerPanelWidget::HandleTooltipCleared(UTCFTooltipSourceWidget* SourceWidget)
{
	if (PlayerUISubsystem)
	{
		PlayerUISubsystem->ClearTooltip(SourceWidget);
	}
}