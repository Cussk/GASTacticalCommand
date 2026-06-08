//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFProductionPanelWidget.h"

#include "Actors/TCFBuildingActor.h"
#include "Components/PanelWidget.h"
#include "Components/TCFBuildingProductionComponent.h"
#include "Components/TCFPlayerResourceBankComponent.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "Data/TCFProductionOptionDefinition.h"
#include "Player/TCFPlayerState.h"
#include "UI/TCFProductionOptionButtonWidget.h"
#include "UI/TCFProductionQueueSlotWidget.h"

void UTCFProductionPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BindSelectionComponent();

	if (ObservedSelectionComponent)
	{
		SetObservedBuilding(ObservedSelectionComponent->GetInspectedBuilding());
	}
	else
	{
		RefreshPanelData();
	}
}

void UTCFProductionPanelWidget::NativeDestruct()
{
	UnbindProductionComponent();
	UnbindSelectionComponent();
	UnbindPooledWidgetDelegates();

	ReleaseOptionButtonsFromIndex(0);
	ReleaseQueueSlotsFromIndex(0);

	Super::NativeDestruct();
}

void UTCFProductionPanelWidget::BindSelectionComponent()
{
	APlayerController* PlayerController = GetOwningPlayer();
	ObservedSelectionComponent = PlayerController
		? PlayerController->FindComponentByClass<UTCFPlayerSelectionComponent>()
		: nullptr;

	if (ObservedSelectionComponent)
	{
		ObservedSelectionComponent->OnInspectedBuildingChanged.AddDynamic(
			this,
			&UTCFProductionPanelWidget::HandleInspectedBuildingChanged);
	}
}

void UTCFProductionPanelWidget::UnbindSelectionComponent()
{
	if (ObservedSelectionComponent)
	{
		ObservedSelectionComponent->OnInspectedBuildingChanged.RemoveDynamic(
			this,
			&UTCFProductionPanelWidget::HandleInspectedBuildingChanged);
	}

	ObservedSelectionComponent = nullptr;
}

void UTCFProductionPanelWidget::SetObservedBuilding(ATCFBuildingActor* Building)
{
	ATCFBuildingActor* NewBuilding = IsValid(Building) ? Building : nullptr;
	if (ObservedBuilding == NewBuilding)
	{
		RefreshPanelData();
		return;
	}

	UnbindProductionComponent();

	ObservedBuilding = NewBuilding;
	ObservedProductionComponent = ObservedBuilding
		? ObservedBuilding->GetProductionComponent()
		: nullptr;

	BindProductionComponent(ObservedProductionComponent);
	RefreshPanelData();
}

void UTCFProductionPanelWidget::BindProductionComponent(
	UTCFBuildingProductionComponent* ProductionComponent)
{
	if (!ProductionComponent)
	{
		return;
	}

	ProductionComponent->OnProductionQueueChanged.AddDynamic(
		this,
		&UTCFProductionPanelWidget::HandleProductionQueueChanged);

	ProductionComponent->OnProductionProgressChanged.AddDynamic(
		this,
		&UTCFProductionPanelWidget::HandleProductionProgressChanged);
}

void UTCFProductionPanelWidget::UnbindProductionComponent()
{
	if (ObservedProductionComponent)
	{
		ObservedProductionComponent->OnProductionQueueChanged.RemoveDynamic(
			this,
			&UTCFProductionPanelWidget::HandleProductionQueueChanged);

		ObservedProductionComponent->OnProductionProgressChanged.RemoveDynamic(
			this,
			&UTCFProductionPanelWidget::HandleProductionProgressChanged);
	}

	ObservedProductionComponent = nullptr;
}

void UTCFProductionPanelWidget::RefreshPanelData()
{
	RebuildProductionOptions();
	RebuildProductionQueue();

	RefreshOptionButtonPool();
	RefreshQueueSlotPool();

	OnProductionPanelDataChanged.Broadcast();
	BP_OnProductionPanelDataChanged();
}

bool UTCFProductionPanelWidget::RequestProductionOption(
	UTCFProductionOptionDefinition* ProductionOption)
{
	ATCFPlayerState* RequestingPlayerState = ResolveRequestingPlayerState();
	if (!ObservedProductionComponent || !ProductionOption || !RequestingPlayerState)
	{
		return false;
	}

	const bool bRequestStarted = ObservedProductionComponent->RequestProduction(
		ProductionOption,
		RequestingPlayerState);

	RefreshPanelData();

	return bRequestStarted;
}

ATCFBuildingActor* UTCFProductionPanelWidget::GetObservedBuilding() const
{
	return IsValid(ObservedBuilding) ? ObservedBuilding : nullptr;
}

bool UTCFProductionPanelWidget::HasObservedProductionBuilding() const
{
	return IsValid(ObservedBuilding) && ObservedProductionComponent != nullptr;
}

void UTCFProductionPanelWidget::GetProductionOptionViewData(
	TArray<FTCFProductionOptionViewData>& OutOptions) const
{
	OutOptions = CachedProductionOptions;
}

void UTCFProductionPanelWidget::GetProductionQueueViewData(
	TArray<FTCFProductionQueueItemViewData>& OutQueue) const
{
	OutQueue = CachedProductionQueue;
}

void UTCFProductionPanelWidget::HandleInspectedBuildingChanged(
	ATCFBuildingActor* InspectedBuilding)
{
	SetObservedBuilding(InspectedBuilding);
}

void UTCFProductionPanelWidget::HandleProductionQueueChanged(
	UTCFBuildingProductionComponent* ProductionComponent)
{
	if (ProductionComponent == ObservedProductionComponent)
	{
		RefreshPanelData();
	}
}

void UTCFProductionPanelWidget::HandleProductionProgressChanged(
	UTCFBuildingProductionComponent* ProductionComponent,
	float ActiveProgressAlpha)
{
	if (ProductionComponent == ObservedProductionComponent)
	{
		RefreshPanelData();
	}
}

void UTCFProductionPanelWidget::HandleProductionOptionButtonClicked(
	UTCFProductionOptionButtonWidget* ButtonWidget,
	UTCFProductionOptionDefinition* ProductionOption)
{
	RequestProductionOption(ProductionOption);
}

void UTCFProductionPanelWidget::RebuildProductionOptions()
{
	CachedProductionOptions.Reset();

	if (!ObservedProductionComponent)
	{
		return;
	}

	ATCFPlayerState* RequestingPlayerState = ResolveRequestingPlayerState();

	TArray<UTCFProductionOptionDefinition*> Options;
	ObservedProductionComponent->GetProductionOptions(Options);

	const bool bHasQueueSpace = ObservedProductionComponent->HasProductionQueueSpace();

	const UTCFPlayerResourceBankComponent* ResourceBank = RequestingPlayerState
		? RequestingPlayerState->GetPlayerResourceBankComponent()
		: nullptr;

	for (UTCFProductionOptionDefinition* Option : Options)
	{
		if (!Option)
		{
			continue;
		}

		FTCFProductionOptionViewData ViewData;
		ViewData.ProductionOption = Option;
		ViewData.DisplayName = Option->GetSafeDisplayName();
		ViewData.Description = Option->Description;
		ViewData.Cost = Option->Cost;
		ViewData.ProductionTime = Option->GetSafeProductionTime();
		ViewData.bHasQueueSpace = bHasQueueSpace;
		ViewData.bCanAfford = ResourceBank
			? ResourceBank->CanAffordResources(Option->Cost)
			: false;

		const bool bCanUseOption = RequestingPlayerState
			&& ObservedProductionComponent->CanUseProductionOption(Option, RequestingPlayerState);

		ViewData.bCanRequest = bCanUseOption
			&& ViewData.bCanAfford
			&& bHasQueueSpace;

		if (!bHasQueueSpace)
		{
			ViewData.DisabledReason = FText::FromString(TEXT("Queue full"));
		}
		else if (!ViewData.bCanAfford)
		{
			ViewData.DisabledReason = FText::FromString(TEXT("Insufficient resources"));
		}
		else if (!bCanUseOption)
		{
			ViewData.DisabledReason = FText::FromString(TEXT("Unavailable"));
		}
		else
		{
			ViewData.DisabledReason = FText::GetEmpty();
		}

		CachedProductionOptions.Add(ViewData);
	}
}

void UTCFProductionPanelWidget::RebuildProductionQueue()
{
	CachedProductionQueue.Reset();

	if (!ObservedProductionComponent)
	{
		return;
	}

	TArray<FTCFProductionQueueItem> QueueItems;
	ObservedProductionComponent->GetProductionQueue(QueueItems);

	for (int32 Index = 0; Index < QueueItems.Num(); ++Index)
	{
		const FTCFProductionQueueItem& QueueItem = QueueItems[Index];

		FTCFProductionQueueItemViewData ViewData;
		ViewData.ProductionOption = QueueItem.ProductionOption;
		ViewData.DisplayName = QueueItem.ProductionOption
			? QueueItem.ProductionOption->GetSafeDisplayName()
			: FText::FromString(TEXT("Invalid"));
		ViewData.CompletedProductionWork = QueueItem.CompletedProductionWork;
		ViewData.RequiredProductionWork = QueueItem.RequiredProductionWork;
		ViewData.ProgressAlpha = QueueItem.GetProgressAlpha();
		ViewData.bIsActiveItem = Index == 0;

		CachedProductionQueue.Add(ViewData);
	}
}

void UTCFProductionPanelWidget::RefreshOptionButtonPool()
{
	if (!ProductionOptionsContainer || !ProductionOptionButtonWidgetClass)
	{
		ReleaseOptionButtonsFromIndex(0);
		return;
	}

	const int32 RequiredOptionCount = CachedProductionOptions.Num();
	EnsureOptionButtonPoolSize(RequiredOptionCount);

	for (int32 Index = 0; Index < OptionButtonPool.Num(); ++Index)
	{
		UTCFProductionOptionButtonWidget* ButtonWidget = OptionButtonPool[Index];
		if (!ButtonWidget)
		{
			continue;
		}

		if (CachedProductionOptions.IsValidIndex(Index))
		{
			ButtonWidget->SetOptionViewData(CachedProductionOptions[Index]);
			ButtonWidget->AcquireFromPool();
		}
		else
		{
			ButtonWidget->ReleaseToPool();
		}
	}
}

void UTCFProductionPanelWidget::RefreshQueueSlotPool()
{
	if (!ProductionQueueContainer || !ProductionQueueSlotWidgetClass)
	{
		ReleaseQueueSlotsFromIndex(0);
		return;
	}

	const int32 EffectiveMaxQueueSize = ObservedProductionComponent
		? ObservedProductionComponent->GetEffectiveMaxQueueSize()
		: 0;

	const int32 VisibleQueueSlotCount = FMath::Max(
		EffectiveMaxQueueSize,
		CachedProductionQueue.Num());

	EnsureQueueSlotPoolSize(VisibleQueueSlotCount);

	for (int32 Index = 0; Index < QueueSlotPool.Num(); ++Index)
	{
		UTCFProductionQueueSlotWidget* QueueSlotWidget = QueueSlotPool[Index];
		if (!QueueSlotWidget)
		{
			continue;
		}

		if (Index >= VisibleQueueSlotCount)
		{
			QueueSlotWidget->ReleaseToPool();
			continue;
		}

		if (CachedProductionQueue.IsValidIndex(Index))
		{
			QueueSlotWidget->SetQueueItemViewData(CachedProductionQueue[Index], Index);
		}
		else
		{
			QueueSlotWidget->SetEmptyQueueSlot(Index);
		}

		QueueSlotWidget->AcquireFromPool();
	}
}

void UTCFProductionPanelWidget::EnsureOptionButtonPoolSize(int32 RequiredCount)
{
	if (!ProductionOptionsContainer || !ProductionOptionButtonWidgetClass)
	{
		return;
	}

	while (OptionButtonPool.Num() < RequiredCount)
	{
		UTCFProductionOptionButtonWidget* NewButton = CreateOptionButtonWidget();
		if (!NewButton)
		{
			return;
		}

		OptionButtonPool.Add(NewButton);
	}
}

void UTCFProductionPanelWidget::EnsureQueueSlotPoolSize(int32 RequiredCount)
{
	if (!ProductionQueueContainer || !ProductionQueueSlotWidgetClass)
	{
		return;
	}

	while (QueueSlotPool.Num() < RequiredCount)
	{
		UTCFProductionQueueSlotWidget* NewSlot = CreateQueueSlotWidget();
		if (!NewSlot)
		{
			return;
		}

		QueueSlotPool.Add(NewSlot);
	}
}

UTCFProductionOptionButtonWidget* UTCFProductionPanelWidget::CreateOptionButtonWidget()
{
	if (!ProductionOptionsContainer || !ProductionOptionButtonWidgetClass)
	{
		return nullptr;
	}

	UTCFProductionOptionButtonWidget* NewButton = CreateWidget<UTCFProductionOptionButtonWidget>(
		GetOwningPlayer(),
		ProductionOptionButtonWidgetClass);

	if (!NewButton)
	{
		return nullptr;
	}

	NewButton->OnProductionOptionButtonClicked.AddDynamic(
		this,
		&UTCFProductionPanelWidget::HandleProductionOptionButtonClicked);

	ProductionOptionsContainer->AddChild(NewButton);
	NewButton->ReleaseToPool();

	return NewButton;
}

UTCFProductionQueueSlotWidget* UTCFProductionPanelWidget::CreateQueueSlotWidget() const
{
	if (!ProductionQueueContainer || !ProductionQueueSlotWidgetClass)
	{
		return nullptr;
	}

	UTCFProductionQueueSlotWidget* NewSlot = CreateWidget<UTCFProductionQueueSlotWidget>(
		GetOwningPlayer(),
		ProductionQueueSlotWidgetClass);

	if (!NewSlot)
	{
		return nullptr;
	}

	ProductionQueueContainer->AddChild(NewSlot);
	NewSlot->ReleaseToPool();

	return NewSlot;
}

void UTCFProductionPanelWidget::ReleaseOptionButtonsFromIndex(int32 FirstIndexToRelease)
{
	for (int32 Index = FirstIndexToRelease; Index < OptionButtonPool.Num(); ++Index)
	{
		if (UTCFProductionOptionButtonWidget* ButtonWidget = OptionButtonPool[Index])
		{
			ButtonWidget->ReleaseToPool();
		}
	}
}

void UTCFProductionPanelWidget::ReleaseQueueSlotsFromIndex(int32 FirstIndexToRelease)
{
	for (int32 Index = FirstIndexToRelease; Index < QueueSlotPool.Num(); ++Index)
	{
		if (UTCFProductionQueueSlotWidget* QueueSlotWidget = QueueSlotPool[Index])
		{
			QueueSlotWidget->ReleaseToPool();
		}
	}
}

void UTCFProductionPanelWidget::UnbindPooledWidgetDelegates()
{
	for (UTCFProductionOptionButtonWidget* ButtonWidget : OptionButtonPool)
	{
		if (ButtonWidget)
		{
			ButtonWidget->OnProductionOptionButtonClicked.RemoveDynamic(
				this,
				&UTCFProductionPanelWidget::HandleProductionOptionButtonClicked);
		}
	}
}

ATCFPlayerState* UTCFProductionPanelWidget::ResolveRequestingPlayerState() const
{
	const APlayerController* PlayerController = GetOwningPlayer();
	return PlayerController
		? Cast<ATCFPlayerState>(PlayerController->PlayerState)
		: nullptr;
}