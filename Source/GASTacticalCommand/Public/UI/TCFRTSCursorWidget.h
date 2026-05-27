//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/TCFRTSControlTypes.h"
#include "TCFRTSCursorWidget.generated.h"

class UTCFRTSHoverContextComponent;

UCLASS()
class GASTACTICALCOMMAND_API UTCFRTSCursorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeCursorWidget(UTCFRTSHoverContextComponent* InHoverContextComponent);

protected:
	virtual void NativeConstruct() override;

	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	TArray<FTCFRTSCursorVisual> CursorVisuals;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|RTS Cursor")
	bool bCorrectForViewportScale = true;

private:
	TWeakObjectPtr<UTCFRTSHoverContextComponent> CachedHoverContextComponent;

	const FTCFRTSCursorVisual* FindCursorVisual(ETCFRTSCursorState CursorState) const;
	FVector2D ConvertScreenPositionToWidgetPosition(FVector2D ScreenPosition) const;
};