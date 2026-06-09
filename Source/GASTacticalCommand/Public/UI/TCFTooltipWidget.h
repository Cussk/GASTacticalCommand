//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TCFTooltipWidget.generated.h"

UCLASS(Abstract)
class GASTACTICALCOMMAND_API UTCFTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TCF|Tooltip")
	const FText& GetTooltipTitle() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Tooltip")
	const FText& GetTooltipSubtitle() const;

protected:
	void SetTooltipHeader(const FText& InTitle, const FText& InSubtitle);
	void ClearTooltipHeader();
	void NotifyTooltipDataChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "TCF|Tooltip")
	void BP_OnTooltipDataChanged();

	virtual void NativeOnTooltipDataChanged();

private:
	UPROPERTY(BlueprintReadOnly, Category = "TCF|Tooltip", meta = (AllowPrivateAccess = true))
	FText TooltipTitle;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Tooltip", meta = (AllowPrivateAccess = true))
	FText TooltipSubtitle;
};