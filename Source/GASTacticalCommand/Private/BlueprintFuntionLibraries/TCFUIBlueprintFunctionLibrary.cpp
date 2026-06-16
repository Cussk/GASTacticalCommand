//Copyright Kyle Cuss and Cuss Programming 2026.

#include "BlueprintFuntionLibraries/TCFUIBlueprintFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "TCFUIBlueprintFunctionLibrary"

FText UTCFUIBlueprintFunctionLibrary::FormatTimeMMSS(const float TimeSeconds)
{
	const int32 TotalSeconds = FMath::Max(0, FMath::CeilToInt(TimeSeconds));

	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;

	return FText::FromString(
		FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)
	);
}

FText UTCFUIBlueprintFunctionLibrary::FormatTimeClock(
	const float TimeSeconds,
	const ETCFUITimeRoundingMode RoundingMode,
	const bool bClampToZero,
	const bool bShowHoursWhenNeeded)
{
	float WorkingSeconds = bClampToZero ? FMath::Max(0.0f, TimeSeconds) : TimeSeconds;

	const bool bIsNegative = WorkingSeconds < 0.0f;
	WorkingSeconds = FMath::Abs(WorkingSeconds);

	int32 TotalSeconds = 0;

	switch (RoundingMode)
	{
	case ETCFUITimeRoundingMode::Floor:
		TotalSeconds = FMath::FloorToInt(WorkingSeconds);
		break;

	case ETCFUITimeRoundingMode::Round:
		TotalSeconds = FMath::RoundToInt(WorkingSeconds);
		break;

	case ETCFUITimeRoundingMode::Ceil:
	default:
		TotalSeconds = FMath::CeilToInt(WorkingSeconds);
		break;
	}

	const FString SignPrefix = bIsNegative ? TEXT("-") : TEXT("");

	const int32 Hours = TotalSeconds / 3600;
	const int32 Minutes = (TotalSeconds % 3600) / 60;
	const int32 Seconds = TotalSeconds % 60;

	if (bShowHoursWhenNeeded && Hours > 0)
	{
		return FText::FromString(
			FString::Printf(TEXT("%s%d:%02d:%02d"), *SignPrefix, Hours, Minutes, Seconds)
		);
	}

	const int32 TotalMinutes = TotalSeconds / 60;

	return FText::FromString(
		FString::Printf(TEXT("%s%02d:%02d"), *SignPrefix, TotalMinutes, Seconds)
	);
}

FText UTCFUIBlueprintFunctionLibrary::FormatCompactNumber(const int32 Value, const int32 FractionalDigits)
{
	const int32 SafeFractionalDigits = FMath::Max(0, FractionalDigits);
	const int64 AbsValue = FMath::Abs(static_cast<int64>(Value));

	float DisplayValue = static_cast<float>(Value);
	FString Suffix;

	if (AbsValue >= 1'000'000)
	{
		DisplayValue = static_cast<float>(Value) / 1'000'000.0f;
		Suffix = TEXT("M");
	}
	else if (AbsValue >= 1'000)
	{
		DisplayValue = static_cast<float>(Value) / 1'000.0f;
		Suffix = TEXT("K");
	}

	FNumberFormattingOptions NumberOptions;
	NumberOptions.UseGrouping = false;
	NumberOptions.MinimumFractionalDigits = 0;
	NumberOptions.MaximumFractionalDigits = SafeFractionalDigits;

	return FText::FromString(
		FText::AsNumber(DisplayValue, &NumberOptions).ToString() + Suffix
	);
}

FText UTCFUIBlueprintFunctionLibrary::FormatResourceDelta(const int32 Delta, const bool bShowPlusForPositive)
{
	if (Delta > 0 && bShowPlusForPositive)
	{
		return FText::FromString(FString::Printf(TEXT("+%d"), Delta));
	}

	return FText::AsNumber(Delta);
}

FText UTCFUIBlueprintFunctionLibrary::FormatPercentFromNormalized(
	const float NormalizedValue,
	const int32 FractionalDigits,
	const bool bClampToZeroOne)
{
	const float SafeValue = bClampToZeroOne
		? FMath::Clamp(NormalizedValue, 0.0f, 1.0f)
		: NormalizedValue;

	const float PercentValue = SafeValue * 100.0f;

	FNumberFormattingOptions NumberOptions;
	NumberOptions.UseGrouping = false;
	NumberOptions.MinimumFractionalDigits = 0;
	NumberOptions.MaximumFractionalDigits = FMath::Max(0, FractionalDigits);

	return FText::FromString(
		FText::AsNumber(PercentValue, &NumberOptions).ToString() + TEXT("%")
	);
}

FText UTCFUIBlueprintFunctionLibrary::FormatCurrentMax(const int32 Current, const int32 Max)
{
	return FText::Format(
		LOCTEXT("CurrentMaxFormat", "{0} / {1}"),
		FText::AsNumber(Current),
		FText::AsNumber(Max)
	);
}

FLinearColor UTCFUIBlueprintFunctionLibrary::SelectEnabledTint(
	const bool bEnabled,
	const FLinearColor EnabledTint,
	const FLinearColor DisabledTint)
{
	return bEnabled ? EnabledTint : DisabledTint;
}

ESlateVisibility UTCFUIBlueprintFunctionLibrary::VisibilityFromBool(
	const bool bVisible,
	const ESlateVisibility FalseVisibility)
{
	return bVisible ? ESlateVisibility::Visible : FalseVisibility;
}

#undef LOCTEXT_NAMESPACE