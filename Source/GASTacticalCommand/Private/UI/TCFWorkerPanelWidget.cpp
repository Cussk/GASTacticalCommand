//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFWorkerPanelWidget.h"

#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Components/TCFRTSCommandRouterComponent.h"
#include "Player/TCFPlayerController.h"
#include "Subsystems/TCFPlayerUISubsystem.h"
#include "TCFGameplayTags.h"

void UTCFWorkerPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!WorkerRoleTag.IsValid())
	{
		WorkerRoleTag = TCFGameplayTags::Squad_Role_Worker;
	}

	BindSelectionComponent();
	RefreshWorkerPanel();
}

void UTCFWorkerPanelWidget::NativeDestruct()
{
	UnbindSelectionComponent();

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