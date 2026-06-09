//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "UI/TCFPooledWidget.h"
#include "TCFTooltipSourceWidget.generated.h"

class UTCFTooltipWidget;
class UTCFTooltipSourceWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnTCFTooltipRequested,
	UTCFTooltipSourceWidget*,
	SourceWidget,
	UTCFTooltipWidget*,
	TooltipWidget);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnTCFTooltipCleared,
	UTCFTooltipSourceWidget*,
	SourceWidget);

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFTooltipSourceWidget : public UTCFPooledWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "TCF|Tooltip")
	FOnTCFTooltipRequested OnTooltipRequested;

	UPROPERTY(BlueprintAssignable, Category = "TCF|Tooltip")
	FOnTCFTooltipCleared OnTooltipCleared;

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnReleasedToPool() override;

	virtual UTCFTooltipWidget* GetTooltipWidgetForSource();
};