//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFResourcePanelWidget.h"

#include "Components/PanelWidget.h"
#include "Components/TCFPlayerResourceBankComponent.h"
#include "Data/TCFResourceUIDefinition.h"
#include "Player/TCFPlayerState.h"
#include "Subsystems/TCFPlayerUISubsystem.h"
#include "UI/TCFResourceAmountWidget.h"

void UTCFResourcePanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RefreshObservedResourceBank();
	RefreshResourcePanel();
}

void UTCFResourcePanelWidget::NativeDestruct()
{
	UnbindObservedResourceBank();
	ReleaseResourceAmountWidgetsFromIndex(0);

	PlayerUISubsystem = nullptr;

	Super::NativeDestruct();
}

void UTCFResourcePanelWidget::SetPlayerUISubsystem(
	UTCFPlayerUISubsystem* InPlayerUISubsystem)
{
	PlayerUISubsystem = InPlayerUISubsystem;

	RefreshObservedResourceBank();
	RefreshResourcePanel();
}

void UTCFResourcePanelWidget::RefreshResourcePanel()
{
	UTCFResourceUIDefinition* ResourceUIDefinition = ResolveResourceUIDefinition();
	if (!ResourceUIDefinition || !ObservedResourceBank || !ResourceListContainer || !ResourceAmountWidgetClass)
	{
		ResourceAmountWidgetsByTag.Reset();
		ReleaseResourceAmountWidgetsFromIndex(0);
		BP_OnResourcePanelRefreshed();
		return;
	}

	TArray<FTCFResourceUIViewData> ResourceViewDataList;
	ResourceUIDefinition->GetAllResourceViewData(ResourceViewDataList);

	EnsureResourceAmountWidgetCount(ResourceViewDataList.Num());

	ResourceAmountWidgetsByTag.Reset();

	for (int32 Index = 0; Index < ResourceAmountWidgets.Num(); ++Index)
	{
		UTCFResourceAmountWidget* ResourceWidget = ResourceAmountWidgets[Index];
		if (!ResourceWidget)
		{
			continue;
		}

		if (!ResourceViewDataList.IsValidIndex(Index))
		{
			ResourceWidget->ClearResourceAmountData();
			continue;
		}

		const FTCFResourceUIViewData& ResourceViewData = ResourceViewDataList[Index];
		const int32 ResourceAmount = ObservedResourceBank->GetResourceAmount(ResourceViewData.ResourceTag);

		ResourceWidget->SetResourceAmountData(ResourceViewData, ResourceAmount);

		if (ResourceViewData.ResourceTag.IsValid())
		{
			ResourceAmountWidgetsByTag.Add(ResourceViewData.ResourceTag, ResourceWidget);
		}
	}

	BP_OnResourcePanelRefreshed();
}

void UTCFResourcePanelWidget::RefreshResourceAmount(FGameplayTag ResourceType)
{
	if (!ResourceType.IsValid() || !ObservedResourceBank)
	{
		return;
	}

	const TObjectPtr<UTCFResourceAmountWidget>* ResourceWidgetPtr = ResourceAmountWidgetsByTag.Find(ResourceType);
	if (!ResourceWidgetPtr || !*ResourceWidgetPtr)
	{
		return;
	}

	FTCFResourceUIViewData ResourceViewData;
	UTCFResourceUIDefinition* ResourceUIDefinition = ResolveResourceUIDefinition();
	if (!ResourceUIDefinition || !ResourceUIDefinition->TryGetResourceViewData(ResourceType, ResourceViewData))
	{
		(*ResourceWidgetPtr)->ClearResourceAmountData();
		return;
	}

	const int32 ResourceAmount = ObservedResourceBank->GetResourceAmount(ResourceType);
	(*ResourceWidgetPtr)->SetResourceAmountData(ResourceViewData, ResourceAmount);
}

UTCFPlayerResourceBankComponent* UTCFResourcePanelWidget::GetObservedResourceBank() const
{
	return ObservedResourceBank;
}

void UTCFResourcePanelWidget::HandleResourceAmountChanged(
	FGameplayTag ResourceType,
	int32 OldAmount,
	int32 NewAmount)
{
	RefreshResourceAmount(ResourceType);
}

void UTCFResourcePanelWidget::RefreshObservedResourceBank()
{
	UTCFPlayerResourceBankComponent* NewResourceBank = nullptr;

	if (ATCFPlayerState* TCFPlayerState = ResolvePlayerState())
	{
		NewResourceBank = TCFPlayerState->GetPlayerResourceBankComponent();
	}

	if (ObservedResourceBank == NewResourceBank)
	{
		return;
	}

	UnbindObservedResourceBank();

	ObservedResourceBank = NewResourceBank;

	BindObservedResourceBank();
}

void UTCFResourcePanelWidget::BindObservedResourceBank()
{
	if (!ObservedResourceBank)
	{
		return;
	}

	ObservedResourceBank->OnResourceAmountChanged.AddDynamic(
		this,
		&UTCFResourcePanelWidget::HandleResourceAmountChanged);
}

void UTCFResourcePanelWidget::UnbindObservedResourceBank()
{
	if (!ObservedResourceBank)
	{
		return;
	}

	ObservedResourceBank->OnResourceAmountChanged.RemoveDynamic(
		this,
		&UTCFResourcePanelWidget::HandleResourceAmountChanged);

	ObservedResourceBank = nullptr;
}

ATCFPlayerState* UTCFResourcePanelWidget::ResolvePlayerState() const
{
	const APlayerController* PlayerController = GetOwningPlayer();
	return PlayerController
		? Cast<ATCFPlayerState>(PlayerController->PlayerState)
		: nullptr;
}

UTCFResourceUIDefinition* UTCFResourcePanelWidget::ResolveResourceUIDefinition() const
{
	return PlayerUISubsystem
		? PlayerUISubsystem->GetResourceUIDefinition()
		: nullptr;
}

void UTCFResourcePanelWidget::EnsureResourceAmountWidgetCount(int32 RequiredCount)
{
	if (!ResourceListContainer || !ResourceAmountWidgetClass)
	{
		return;
	}

	while (ResourceAmountWidgets.Num() < RequiredCount)
	{
		UTCFResourceAmountWidget* NewWidget = CreateResourceAmountWidget();
		if (!NewWidget)
		{
			return;
		}

		ResourceAmountWidgets.Add(NewWidget);
	}
}

UTCFResourceAmountWidget* UTCFResourcePanelWidget::CreateResourceAmountWidget()
{
	if (!ResourceListContainer || !ResourceAmountWidgetClass)
	{
		return nullptr;
	}

	UTCFResourceAmountWidget* NewWidget = CreateWidget<UTCFResourceAmountWidget>(
		GetOwningPlayer(),
		ResourceAmountWidgetClass);

	if (!NewWidget)
	{
		return nullptr;
	}

	ResourceListContainer->AddChild(NewWidget);
	NewWidget->ClearResourceAmountData();
	BindResourceAmountWidget(NewWidget);

	return NewWidget;
}

void UTCFResourcePanelWidget::ReleaseResourceAmountWidgetsFromIndex(int32 FirstIndexToRelease)
{
	for (int32 Index = FirstIndexToRelease; Index < ResourceAmountWidgets.Num(); ++Index)
	{
		if (UTCFResourceAmountWidget* ResourceWidget = ResourceAmountWidgets[Index])
		{
			ResourceWidget->ClearResourceAmountData();
			UnbindResourceAmountWidget(ResourceWidget);
		}
	}

	if (FirstIndexToRelease <= 0)
	{
		ResourceAmountWidgetsByTag.Reset();
	}
}

void UTCFResourcePanelWidget::BindResourceAmountWidget(UTCFResourceAmountWidget* ResourceWidget)
{
	if (!ResourceWidget)
	{
		return;
	}

	ResourceWidget->OnTooltipRequested.AddUniqueDynamic(
		this,
		&UTCFResourcePanelWidget::HandleTooltipRequested);

	ResourceWidget->OnTooltipCleared.AddUniqueDynamic(
		this,
		&UTCFResourcePanelWidget::HandleTooltipCleared);
}

void UTCFResourcePanelWidget::UnbindResourceAmountWidget(
	UTCFResourceAmountWidget* ResourceWidget)
{
	if (!ResourceWidget)
	{
		return;
	}

	ResourceWidget->OnTooltipRequested.RemoveDynamic(
		this,
		&UTCFResourcePanelWidget::HandleTooltipRequested);

	ResourceWidget->OnTooltipCleared.RemoveDynamic(
		this,
		&UTCFResourcePanelWidget::HandleTooltipCleared);
}

void UTCFResourcePanelWidget::HandleTooltipRequested(UTCFTooltipSourceWidget* SourceWidget, UTCFTooltipWidget* TCFTooltipWidget)
{
	if (PlayerUISubsystem)
	{
		PlayerUISubsystem->RequestTooltip(SourceWidget, TCFTooltipWidget);
	}
}

void UTCFResourcePanelWidget::HandleTooltipCleared(UTCFTooltipSourceWidget* SourceWidget)
{
	if (PlayerUISubsystem)
	{
		PlayerUISubsystem->ClearTooltip(SourceWidget);
	}
}