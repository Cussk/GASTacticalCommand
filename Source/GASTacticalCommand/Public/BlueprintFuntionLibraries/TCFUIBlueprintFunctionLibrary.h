//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/SlateWrapperTypes.h"
#include "TCFUIBlueprintFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class ETCFUITimeRoundingMode : uint8
{
	Floor UMETA(DisplayName = "Floor"),
	Ceil UMETA(DisplayName = "Ceil"),
	Round UMETA(DisplayName = "Round")
};

UCLASS()
class GASTACTICALCOMMAND_API UTCFUIBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Best default for countdown timers. Example: 65.2 -> "01:06"
	UFUNCTION(BlueprintPure, Category = "TCF|UI|Formatting", meta = (DisplayName = "Format Time MM:SS", CompactNodeTitle = "MM:SS"))
	static FText FormatTimeMMSS(float TimeSeconds);

	// More flexible clock formatter. Can show H:MM:SS once time exceeds an hour.
	UFUNCTION(BlueprintPure, Category = "TCF|UI|Formatting", meta = (DisplayName = "Format Time Clock", AdvancedDisplay = "RoundingMode,bClampToZero,bShowHoursWhenNeeded"))
	static FText FormatTimeClock(
		float TimeSeconds,
		ETCFUITimeRoundingMode RoundingMode = ETCFUITimeRoundingMode::Ceil,
		bool bClampToZero = true,
		bool bShowHoursWhenNeeded = true
	);

	// Example: 950 -> "950", 1200 -> "1.2K", 2500000 -> "2.5M"
	UFUNCTION(BlueprintPure, Category = "TCF|UI|Formatting", meta = (DisplayName = "Format Compact Number", AdvancedDisplay = "FractionalDigits"))
	static FText FormatCompactNumber(int32 Value, int32 FractionalDigits = 1);

	// Example: 25 -> "+25", -10 -> "-10", 0 -> "0"
	UFUNCTION(BlueprintPure, Category = "TCF|UI|Formatting", meta = (DisplayName = "Format Resource Delta", AdvancedDisplay = "bShowPlusForPositive"))
	static FText FormatResourceDelta(int32 Delta, bool bShowPlusForPositive = true);

	// Example: 0.75 -> "75%"
	UFUNCTION(BlueprintPure, Category = "TCF|UI|Formatting", meta = (DisplayName = "Format Percent From Normalized", AdvancedDisplay = "FractionalDigits,bClampToZeroOne"))
	static FText FormatPercentFromNormalized(float NormalizedValue, int32 FractionalDigits = 0, bool bClampToZeroOne = true);

	// Example: 3, 5 -> "3 / 5"
	UFUNCTION(BlueprintPure, Category = "TCF|UI|Formatting", meta = (DisplayName = "Format Current / Max"))
	static FText FormatCurrentMax(int32 Current, int32 Max);

	// Useful for enabled/disabled icon tinting.
	UFUNCTION(BlueprintPure, Category = "TCF|UI|State", meta = (DisplayName = "Select Enabled Tint"))
	static FLinearColor SelectEnabledTint(bool bEnabled, FLinearColor EnabledTint, FLinearColor DisabledTint);

	// Useful for simple UI gating without branching in every widget graph.
	UFUNCTION(BlueprintPure, Category = "TCF|UI|State", meta = (DisplayName = "Visibility From Bool"))
	static ESlateVisibility VisibilityFromBool(bool bVisible, ESlateVisibility FalseVisibility = ESlateVisibility::Collapsed);
};