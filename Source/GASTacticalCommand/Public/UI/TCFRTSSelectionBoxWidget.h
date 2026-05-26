//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TCFRTSSelectionBoxWidget.generated.h"

class UTCFRTSSelectionBoxComponent;

UCLASS()
class GASTACTICALCOMMAND_API UTCFRTSSelectionBoxWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeSelectionBoxComponent(UTCFRTSSelectionBoxComponent* InSelectionBoxComponent);

protected:
	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Selection Box")
	FLinearColor FillColor = FLinearColor(0.1f, 0.65f, 1.0f, 0.12f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Selection Box")
	FLinearColor BorderColor = FLinearColor(0.1f, 0.85f, 1.0f, 0.85f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Selection Box", meta = (ClampMin = "1.0"))
	float BorderThickness = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TCF|Selection Box")
	bool bCorrectForViewportScale = true;

private:
	void DrawSelectionRectangle(
		const FGeometry& AllottedGeometry,
		FSlateWindowElementList& OutDrawElements,
		int32& LayerId,
		FVector2D ScreenStart,
		FVector2D ScreenEnd) const;

	FVector2D ConvertScreenPositionToWidgetPosition(FVector2D ScreenPosition) const;
	
	TWeakObjectPtr<UTCFRTSSelectionBoxComponent> CachedSelectionBoxComponent;
};