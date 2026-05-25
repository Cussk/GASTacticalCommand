//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Debug/TCFDebugTypes.h"
#include "TCFDebugHUDWidget.generated.h"

class STextBlock;
class SVerticalBox;

UCLASS()
class GASTACTICALCOMMANDDEBUG_API UTCFDebugHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TCF|Debug")
	void SetDebugSnapshot(const FTCFDebugSquadSnapshot& InSnapshot);

	UFUNCTION(BlueprintPure, Category = "TCF|Debug")
	const FTCFDebugSquadSnapshot& GetDebugSnapshot() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	UPROPERTY()
	FTCFDebugSquadSnapshot Snapshot;

	TSharedPtr<STextBlock> HeaderText;
	TSharedPtr<STextBlock> SquadText;
	TSharedPtr<STextBlock> AttributeText;
	TSharedPtr<STextBlock> TagText;
	TSharedPtr<STextBlock> AbilityText;
	TSharedPtr<STextBlock> EffectText;
	TSharedPtr<STextBlock> OrderText;
	TSharedPtr<STextBlock> AffiliationText;
	TSharedPtr<STextBlock> RelationshipText;
	TSharedPtr<STextBlock> CapturePointText;

	void RefreshText() const;

	FText BuildSquadText() const;
	FText BuildAttributeText() const;
	FText BuildTagText() const;
	FText BuildAbilityText() const;
	FText BuildEffectText() const;
	FText BuildOrderText() const;
	FText BuildAffiliationText() const;
	FText BuildRelationshipText() const;
	FText BuildCapturePointText() const;

	static FString JoinLines(const TArray<FString>& Lines);
};
