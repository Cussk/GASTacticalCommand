//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFPooledWidget.h"

void UTCFPooledWidget::AcquireFromPool()
{
	if (bPoolActive)
	{
		SetVisibility(ESlateVisibility::Visible);
		return;
	}

	bPoolActive = true;
	SetVisibility(ESlateVisibility::Visible);

	NativeOnAcquiredFromPool();
	BP_OnAcquiredFromPool();
}

void UTCFPooledWidget::ReleaseToPool()
{
	if (!bPoolActive)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	bPoolActive = false;

	NativeOnReleasedToPool();
	BP_OnReleasedToPool();

	SetVisibility(ESlateVisibility::Collapsed);
}

bool UTCFPooledWidget::IsPoolActive() const
{
	return bPoolActive;
}

void UTCFPooledWidget::NativeOnAcquiredFromPool()
{
}

void UTCFPooledWidget::NativeOnReleasedToPool()
{
}