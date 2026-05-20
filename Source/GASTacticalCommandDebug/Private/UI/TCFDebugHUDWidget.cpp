//Copyright Kyle Cuss and Cuss Programming 2026.

#include "UI/TCFDebugHUDWidget.h"

#include "Styling/CoreStyle.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"

void UTCFDebugHUDWidget::SetDebugSnapshot(const FTCFDebugSquadSnapshot& InSnapshot)
{
	Snapshot = InSnapshot;
	RefreshText();
}

const FTCFDebugSquadSnapshot& UTCFDebugHUDWidget::GetDebugSnapshot() const
{
	return Snapshot;
}

TSharedRef<SWidget> UTCFDebugHUDWidget::RebuildWidget()
{
	const FSlateColor HeaderColor(FLinearColor(0.25f, 0.75f, 1.0f, 1.0f));
	const FSlateColor SectionColor(FLinearColor(0.75f, 0.85f, 1.0f, 1.0f));
	const FSlateColor BodyColor(FLinearColor(0.88f, 0.90f, 0.92f, 1.0f));

	return SNew(SOverlay)

		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(16.0f)
		[
			SNew(SBox)
			.WidthOverride(460.0f)
			.MaxDesiredHeight(620.0f)
			[
				SNew(SBorder)
				.Padding(2.0f)
				.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
				.BorderBackgroundColor(FSlateColor(FLinearColor(0.10f, 0.45f, 0.75f, 0.85f)))
				[
					SNew(SBorder)
					.Padding(10.0f)
					.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.BorderBackgroundColor(FSlateColor(FLinearColor(0.01f, 0.012f, 0.018f, 0.92f)))
					[
						SNew(SScrollBox)

						+ SScrollBox::Slot()
						.Padding(0.0f, 0.0f, 0.0f, 8.0f)
						[
							SAssignNew(HeaderText, STextBlock)
							.Text(FText::FromString(TEXT("TCF DEBUG HUD")))
							.ColorAndOpacity(HeaderColor)
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
						]

						+ SScrollBox::Slot()
						.Padding(0.0f, 0.0f, 0.0f, 10.0f)
						[
							SAssignNew(SquadText, STextBlock)
							.Text(FText::FromString(TEXT("Selected Squad\nNone")))
							.ColorAndOpacity(BodyColor)
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
							.AutoWrapText(true)
							.WrapTextAt(430.0f)
						]

						+ SScrollBox::Slot()
						.Padding(0.0f, 0.0f, 0.0f, 10.0f)
						[
							SAssignNew(AffiliationText, STextBlock)
							.ColorAndOpacity(BodyColor)
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
							.AutoWrapText(true)
							.WrapTextAt(430.0f)
						]

						+ SScrollBox::Slot()
						.Padding(0.0f, 0.0f, 0.0f, 10.0f)
						[
							SAssignNew(AttributeText, STextBlock)
							.ColorAndOpacity(BodyColor)
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
							.AutoWrapText(true)
							.WrapTextAt(430.0f)
						]

						+ SScrollBox::Slot()
						.Padding(0.0f, 0.0f, 0.0f, 10.0f)
						[
							SAssignNew(TagText, STextBlock)
							.ColorAndOpacity(SectionColor)
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
							.AutoWrapText(true)
							.WrapTextAt(430.0f)
						]

						+ SScrollBox::Slot()
						.Padding(0.0f, 0.0f, 0.0f, 10.0f)
						[
							SAssignNew(AbilityText, STextBlock)
							.ColorAndOpacity(BodyColor)
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
							.AutoWrapText(true)
							.WrapTextAt(430.0f)
						]

						+ SScrollBox::Slot()
						.Padding(0.0f, 0.0f, 0.0f, 10.0f)
						[
							SAssignNew(EffectText, STextBlock)
							.ColorAndOpacity(BodyColor)
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
							.AutoWrapText(true)
							.WrapTextAt(430.0f)
						]

						+ SScrollBox::Slot()
						.Padding(0.0f, 0.0f, 0.0f, 10.0f)
						[
							SAssignNew(RelationshipText, STextBlock)
							.ColorAndOpacity(SectionColor)
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
							.AutoWrapText(true)
							.WrapTextAt(430.0f)
						]

						+ SScrollBox::Slot()
						.Padding(0.0f)
						[
							SAssignNew(OrderText, STextBlock)
							.ColorAndOpacity(BodyColor)
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
							.AutoWrapText(true)
							.WrapTextAt(430.0f)
						]
					]
				]
			]
		];
}

void UTCFDebugHUDWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	HeaderText.Reset();
	SquadText.Reset();
	AttributeText.Reset();
	TagText.Reset();
	AbilityText.Reset();
	EffectText.Reset();
	OrderText.Reset();
	AffiliationText.Reset();
	RelationshipText.Reset();
}

void UTCFDebugHUDWidget::RefreshText() const
{
	if (SquadText)
	{
		SquadText->SetText(BuildSquadText());
	}

	if (AttributeText)
	{
		AttributeText->SetText(BuildAttributeText());
	}

	if (TagText)
	{
		TagText->SetText(BuildTagText());
	}

	if (AbilityText)
	{
		AbilityText->SetText(BuildAbilityText());
	}

	if (EffectText)
	{
		EffectText->SetText(BuildEffectText());
	}
	
	if (AffiliationText)
	{
		AffiliationText->SetText(BuildAffiliationText());
	}

	if (RelationshipText)
	{
		RelationshipText->SetText(BuildRelationshipText());
	}

	if (OrderText)
	{
		OrderText->SetText(BuildOrderText());

		if (Snapshot.LastOrder.bHasOrder)
		{
			const FLinearColor ResultColor = Snapshot.LastOrder.bSuccess
				? FLinearColor(0.35f, 1.0f, 0.45f, 1.0f)
				: FLinearColor(1.0f, 0.25f, 0.22f, 1.0f);

			OrderText->SetColorAndOpacity(FSlateColor(ResultColor));
		}
		else
		{
			OrderText->SetColorAndOpacity(FSlateColor(FLinearColor(0.88f, 0.90f, 0.92f, 1.0f)));
		}
	}
}

FText UTCFDebugHUDWidget::BuildSquadText() const
{
	if (!Snapshot.bHasSelectedSquad)
	{
		return FText::FromString(TEXT("━━ SELECTED SQUAD ━━\nNone"));
	}

	return FText::FromString(FString::Printf(
		TEXT("━━ SELECTED SQUAD ━━\nName: %s\nActor: %s\nRole: %s\nInitialized: %s"),
		*Snapshot.DisplayName.ToString(),
		*Snapshot.ActorName,
		*Snapshot.RoleTag.ToString(),
		Snapshot.bInitialized ? TEXT("true") : TEXT("false")));
}

FText UTCFDebugHUDWidget::BuildAttributeText() const
{
	return FText::FromString(FString::Printf(
		TEXT("━━ ATTRIBUTES ━━\n%s"),
		*JoinLines(Snapshot.AttributeLines)));
}

FText UTCFDebugHUDWidget::BuildTagText() const
{
	const FString Tags = Snapshot.OwnedTags.IsEmpty()
		? TEXT("None")
		: Snapshot.OwnedTags;

	return FText::FromString(FString::Printf(
		TEXT("━━ OWNED TAGS ━━\n%s"),
		*Tags));
}

FText UTCFDebugHUDWidget::BuildAbilityText() const
{
	return FText::FromString(FString::Printf(
		TEXT("━━ GRANTED ABILITIES ━━\n%s"),
		*JoinLines(Snapshot.AbilityLines)));
}

FText UTCFDebugHUDWidget::BuildEffectText() const
{
	return FText::FromString(FString::Printf(
		TEXT("━━ ACTIVE EFFECTS ━━\n%s"),
		*JoinLines(Snapshot.ActiveEffectLines)));
}

FText UTCFDebugHUDWidget::BuildOrderText() const
{
	if (!Snapshot.LastOrder.bHasOrder)
	{
		return FText::FromString(TEXT("━━ LAST ORDER ━━\nNone"));
	}

	const FString BlockingTags = Snapshot.LastOrder.BlockingTags.IsEmpty()
		? TEXT("None")
		: Snapshot.LastOrder.BlockingTags.ToStringSimple();

	const FString Reason = Snapshot.LastOrder.FailureReason.IsEmpty()
		? TEXT("None")
		: Snapshot.LastOrder.FailureReason.ToString();

	return FText::FromString(FString::Printf(
		TEXT("━━ LAST ORDER ━━\nSeq: %d\nOrder: %s\nSource: %s\nTarget: %s\nSuccess: %s\nResult: %s\nBlocking Tags: %s\nReason: %s"),
		Snapshot.LastOrder.OrderSequence,
		*Snapshot.LastOrder.OrderTag.ToString(),
		*Snapshot.LastOrder.SourceName,
		*Snapshot.LastOrder.TargetSummary,
		Snapshot.LastOrder.bSuccess ? TEXT("true") : TEXT("false"),
		*Snapshot.LastOrder.ResultTag.ToString(),
		*BlockingTags,
		*Reason));
}

FText UTCFDebugHUDWidget::BuildAffiliationText() const
{
	if (!Snapshot.bHasSelectedSquad)
	{
		return FText::FromString(TEXT("━━ AFFILIATION ━━\nNone"));
	}

	if (!Snapshot.bHasAffiliation)
	{
		return FText::FromString(TEXT("━━ AFFILIATION ━━\nNo Affiliation Component"));
	}

	const FString FactionString = Snapshot.FactionTag.IsValid()
		? Snapshot.FactionTag.ToString()
		: TEXT("None");

	return FText::FromString(FString::Printf(
		TEXT("━━ AFFILIATION ━━\nOwnerId: %d\nTeamId: %d\nFaction: %s"),
		Snapshot.OwnerId,
		Snapshot.TeamId,
		*FactionString));
}

FText UTCFDebugHUDWidget::BuildRelationshipText() const
{
	return FText::FromString(FString::Printf(
		TEXT("━━ NEARBY RELATIONSHIPS ━━\n%s"),
		*JoinLines(Snapshot.RelationshipLines)));
}

FString UTCFDebugHUDWidget::JoinLines(const TArray<FString>& Lines)
{
	if (Lines.IsEmpty())
	{
		return TEXT("None");
	}

	FString Result;
	for (const FString& Line : Lines)
	{
		if (!Result.IsEmpty())
		{
			Result += LINE_TERMINATOR;
		}

		Result += Line;
	}

	return Result;
}
