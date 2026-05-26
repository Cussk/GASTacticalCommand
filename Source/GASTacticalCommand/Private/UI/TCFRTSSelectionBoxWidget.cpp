//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFRTSSelectionBoxWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/TCFRTSSelectionBoxComponent.h"
#include "Player/TCFPlayerController.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

void UTCFRTSSelectionBoxWidget::InitializeSelectionBoxComponent(UTCFRTSSelectionBoxComponent* InSelectionBoxComponent)
{
	CachedSelectionBoxComponent = InSelectionBoxComponent;
}

int32 UTCFRTSSelectionBoxWidget::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	int32 ResultLayerId = Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);

	const UTCFRTSSelectionBoxComponent* SelectionBoxComponent = CachedSelectionBoxComponent.Get();
	if (!SelectionBoxComponent || !SelectionBoxComponent->IsDraggingSelection())
	{
		return ResultLayerId;
	}

	FVector2D ScreenStart;
	FVector2D ScreenEnd;
	SelectionBoxComponent->GetSelectionRectangle(ScreenStart, ScreenEnd);

	DrawSelectionRectangle(
		AllottedGeometry,
		OutDrawElements,
		ResultLayerId,
		ScreenStart,
		ScreenEnd);

	return ResultLayerId;
}

void UTCFRTSSelectionBoxWidget::DrawSelectionRectangle(
	const FGeometry& AllottedGeometry,
	FSlateWindowElementList& OutDrawElements,
	int32& LayerId,
	FVector2D ScreenStart,
	FVector2D ScreenEnd) const
{
	const FVector2D LocalStart = ConvertScreenPositionToWidgetPosition(ScreenStart);
	const FVector2D LocalEnd = ConvertScreenPositionToWidgetPosition(ScreenEnd);

	const FVector2D MinPosition(
		FMath::Min(LocalStart.X, LocalEnd.X),
		FMath::Min(LocalStart.Y, LocalEnd.Y));

	const FVector2D MaxPosition(
		FMath::Max(LocalStart.X, LocalEnd.X),
		FMath::Max(LocalStart.Y, LocalEnd.Y));

	const FVector2D Size = MaxPosition - MinPosition;
	if (Size.X <= 0.0f || Size.Y <= 0.0f)
	{
		return;
	}

	const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush(TEXT("WhiteBrush"));
	if (!WhiteBrush)
	{
		return;
	}

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		++LayerId,
		AllottedGeometry.ToPaintGeometry(Size, FSlateLayoutTransform(MinPosition)),
		WhiteBrush,
		ESlateDrawEffect::None,
		FillColor);

	const float Thickness = FMath::Max(1.0f, BorderThickness);

	const FVector2D TopPosition = MinPosition;
	const FVector2D TopSize(Size.X, Thickness);

	const FVector2D BottomPosition(MinPosition.X, MaxPosition.Y - Thickness);
	const FVector2D BottomSize(Size.X, Thickness);

	const FVector2D LeftPosition = MinPosition;
	const FVector2D LeftSize(Thickness, Size.Y);

	const FVector2D RightPosition(MaxPosition.X - Thickness, MinPosition.Y);
	const FVector2D RightSize(Thickness, Size.Y);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		++LayerId,
		AllottedGeometry.ToPaintGeometry(TopSize, FSlateLayoutTransform(TopPosition)),
		WhiteBrush,
		ESlateDrawEffect::None,
		BorderColor);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(BottomSize, FSlateLayoutTransform(BottomPosition)),
		WhiteBrush,
		ESlateDrawEffect::None,
		BorderColor);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(LeftSize, FSlateLayoutTransform(LeftPosition)),
		WhiteBrush,
		ESlateDrawEffect::None,
		BorderColor);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(RightSize, FSlateLayoutTransform(RightPosition)),
		WhiteBrush,
		ESlateDrawEffect::None,
		BorderColor);
}

FVector2D UTCFRTSSelectionBoxWidget::ConvertScreenPositionToWidgetPosition(FVector2D ScreenPosition) const
{
	if (!bCorrectForViewportScale)
	{
		return ScreenPosition;
	}

	const float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
	if (ViewportScale <= 0.0f)
	{
		return ScreenPosition;
	}

	return ScreenPosition / ViewportScale;
}