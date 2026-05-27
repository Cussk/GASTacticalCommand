//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFRTSCursorWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/TCFRTSHoverContextComponent.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "Rendering/DrawElements.h"

void UTCFRTSCursorWidget::InitializeCursorWidget(UTCFRTSHoverContextComponent* InHoverContextComponent)
{
	CachedHoverContextComponent = InHoverContextComponent;
}

void UTCFRTSCursorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

int32 UTCFRTSCursorWidget::NativePaint(
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

	const UTCFRTSHoverContextComponent* HoverContextComponent = CachedHoverContextComponent.Get();
	if (!HoverContextComponent)
	{
		return ResultLayerId;
	}

	const FTCFRTSCursorVisual* CursorVisual = FindCursorVisual(HoverContextComponent->GetEffectiveCursorState());
	if (!CursorVisual || !CursorVisual->CursorTexture)
	{
		return ResultLayerId;
	}

	const APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		return ResultLayerId;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!PlayerController->GetMousePosition(MouseX, MouseY))
	{
		return ResultLayerId;
	}

	const FVector2D MousePosition = ConvertScreenPositionToWidgetPosition(FVector2D(MouseX, MouseY));
	const FVector2D DrawSize = CursorVisual->DrawSize;
	const FVector2D DrawPosition = MousePosition - CursorVisual->HotSpot;

	FSlateBrush CursorBrush;
	CursorBrush.SetResourceObject(CursorVisual->CursorTexture);
	CursorBrush.ImageSize = DrawSize;
	CursorBrush.DrawAs = ESlateBrushDrawType::Image;

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		++ResultLayerId,
		AllottedGeometry.ToPaintGeometry(DrawSize, FSlateLayoutTransform(DrawPosition)),
		&CursorBrush,
		ESlateDrawEffect::None,
		CursorVisual->Tint);

	return ResultLayerId;
}

const FTCFRTSCursorVisual* UTCFRTSCursorWidget::FindCursorVisual(ETCFRTSCursorState CursorState) const
{
	for (const FTCFRTSCursorVisual& CursorVisual : CursorVisuals)
	{
		if (CursorVisual.CursorState == CursorState)
		{
			return &CursorVisual;
		}
	}

	for (const FTCFRTSCursorVisual& CursorVisual : CursorVisuals)
	{
		if (CursorVisual.CursorState == ETCFRTSCursorState::Default)
		{
			return &CursorVisual;
		}
	}

	return nullptr;
}

FVector2D UTCFRTSCursorWidget::ConvertScreenPositionToWidgetPosition(FVector2D ScreenPosition) const
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