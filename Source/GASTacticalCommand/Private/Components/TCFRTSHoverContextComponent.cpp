//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFRTSHoverContextComponent.h"

#include "TCFGameplayTags.h"
#include "Actors/TCFBuildingActor.h"
#include "Actors/TCFCapturePointActor.h"
#include "Actors/TCFResourceNodeActor.h"
#include "Actors/TCFSquadActor.h"
#include "Components/TCFPlayerSelectionComponent.h"
#include "GameFramework/PlayerController.h"
#include "Subsystems/TCFRelationshipSubsystem.h"
#include "UI/TCFRTSCursorWidget.h"

UTCFRTSHoverContextComponent::UTCFRTSHoverContextComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SetIsReplicatedByDefault(false);
	
	CursorWidgetClass = UTCFRTSCursorWidget::StaticClass();
}

void UTCFRTSHoverContextComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());

	SelectionComponent = GetOwner()
		? GetOwner()->FindComponentByClass<UTCFPlayerSelectionComponent>()
		: nullptr;

	RelationshipSubsystem = GetWorld()
		? GetWorld()->GetSubsystem<UTCFRelationshipSubsystem>()
		: nullptr;
	
	CreateCursorWidget();
	ForceRefreshHoverContext();
}

void UTCFRTSHoverContextComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RefreshHoverContext();
}

const FTCFRTSHoverContext& UTCFRTSHoverContextComponent::GetCurrentHoverContext() const
{
	return CurrentHoverContext;
}

void UTCFRTSHoverContextComponent::ForceRefreshHoverContext()
{
	RefreshHoverContext();
}

void UTCFRTSHoverContextComponent::SetCursorOverride(ETCFRTSCursorState NewCursorState)
{
	bHasCursorOverride = true;
	CursorOverrideState = NewCursorState;
}

void UTCFRTSHoverContextComponent::ClearCursorOverride()
{
	bHasCursorOverride = false;
	CursorOverrideState = ETCFRTSCursorState::Default;
}

ETCFRTSCursorState UTCFRTSHoverContextComponent::GetEffectiveCursorState() const
{
	return bHasCursorOverride
		? CursorOverrideState
		: CurrentHoverContext.CursorState;
}

void UTCFRTSHoverContextComponent::RefreshHoverContext()
{
	FHitResult HitResult;
	FTCFRTSHoverContext NewHoverContext;

	if (TraceHover(HitResult))
	{
		NewHoverContext = BuildHoverContextFromHit(HitResult);
	}
	else
	{
		NewHoverContext.CursorState = ETCFRTSCursorState::Default;
		NewHoverContext.TargetType = ETCFRTSHoverTargetType::None;
	}

	const bool bHoverContextChanged = !AreHoverContextsEquivalent(CurrentHoverContext, NewHoverContext);

	CurrentHoverContext = NewHoverContext;

	if (bHoverContextChanged)
	{
		OnHoverContextChanged.Broadcast(CurrentHoverContext);
	}
}

bool UTCFRTSHoverContextComponent::TraceHover(FHitResult& OutHitResult) const
{
	if (!PlayerController)
	{
		return false;
	}

	return PlayerController->GetHitResultUnderCursor(
		HoverTraceChannel,
		bTraceComplex,
		OutHitResult);
}

FTCFRTSHoverContext UTCFRTSHoverContextComponent::BuildHoverContextFromHit(const FHitResult& HitResult) const
{
	FTCFRTSHoverContext HoverContext;
	HoverContext.bHasHit = HitResult.bBlockingHit;
	HoverContext.HoveredActor = HitResult.GetActor();
	HoverContext.WorldLocation = HitResult.ImpactPoint.IsNearlyZero()
		? HitResult.Location
		: HitResult.ImpactPoint;

	FVector ResolvedNormal = HitResult.ImpactNormal;
	if (ResolvedNormal.IsNearlyZero())
	{
		ResolvedNormal = HitResult.Normal;
	}

	if (ResolvedNormal.IsNearlyZero())
	{
		ResolvedNormal = FVector::UpVector;
	}

	HoverContext.WorldNormal = ResolvedNormal.GetSafeNormal();
	HoverContext.bHasPrimarySelection = SelectionComponent && SelectionComponent->GetPrimarySelectedSquad();

	AActor* HitActor = HitResult.GetActor();
	HoverContext.TargetType = ResolveTargetType(HitActor);
	HoverContext.RelationshipToPrimarySelection = ResolveRelationshipToPrimarySelection(HitActor);

	const ATCFSquadActor* HoveredSquad = Cast<ATCFSquadActor>(HitActor);
	HoverContext.CursorState = ResolveCursorState(HoverContext, HoveredSquad);

	return HoverContext;
}

ETCFRTSHoverTargetType UTCFRTSHoverContextComponent::ResolveTargetType(const AActor* HitActor) const
{
	if (!HitActor)
	{
		return ETCFRTSHoverTargetType::Ground;
	}

	if (HitActor->IsA<ATCFSquadActor>())
	{
		return ETCFRTSHoverTargetType::Squad;
	}

	if (HitActor->IsA<ATCFCapturePointActor>())
	{
		return ETCFRTSHoverTargetType::CapturePoint;
	}
	
	if (HitActor->IsA<ATCFResourceNodeActor>())
	{
		return ETCFRTSHoverTargetType::ResourceNode;
	}
	
	if (HitActor->IsA<ATCFBuildingActor>())
	{
		return ETCFRTSHoverTargetType::Building;
	}

	return ETCFRTSHoverTargetType::Ground;
}

ETCFRTSCursorState UTCFRTSHoverContextComponent::ResolveCursorState(
	const FTCFRTSHoverContext& HoverContext,
	const ATCFSquadActor* HoveredSquad) const
{
	switch (HoverContext.TargetType)
	{
	case ETCFRTSHoverTargetType::Squad:
		if (!HoveredSquad)
		{
			return ETCFRTSCursorState::InvalidTarget;
		}

		if (!HoverContext.bHasPrimarySelection)
		{
			return ETCFRTSCursorState::SelectableOwn;
		}

		switch (HoverContext.RelationshipToPrimarySelection)
		{
		case ETCFSquadRelationship::Own:
			return ETCFRTSCursorState::SelectableOwn;

		case ETCFSquadRelationship::Friendly:
			return ETCFRTSCursorState::SelectableFriendly;

		case ETCFSquadRelationship::Enemy:
			return ETCFRTSCursorState::AttackEnemy;

		case ETCFSquadRelationship::Neutral:
		default:
			return ETCFRTSCursorState::InvalidTarget;
		}

	case ETCFRTSHoverTargetType::CapturePoint:
		return ETCFRTSCursorState::CapturePoint;

	case ETCFRTSHoverTargetType::ResourceNode:
		return ETCFRTSCursorState::ResourceNode;

	case ETCFRTSHoverTargetType::Building:
		{
			const ATCFBuildingActor* Building = Cast<ATCFBuildingActor>(HoverContext.HoveredActor);
			if (!Building)
			{
				return ETCFRTSCursorState::InvalidTarget;
			}
			
			if (HoverContext.RelationshipToPrimarySelection == ETCFSquadRelationship::Enemy
				&& Building->IsAlive())
			{
				return ETCFRTSCursorState::AttackEnemy;
			}

			if (Building->GetBuildingRoleTags().HasTagExact(TCFGameplayTags::Building_Role_Research))
			{
				return ETCFRTSCursorState::ResearchBuilding;
			}

			if (Building->GetBuildingRoleTags().HasTagExact(TCFGameplayTags::Building_Role_Production))
			{
				return ETCFRTSCursorState::ProductionBuilding;
			}

			return ETCFRTSCursorState::BuildingDetails;
		}

	case ETCFRTSHoverTargetType::Ground:
		return SelectionComponent && SelectionComponent->HasSelectedSquads()
			? ETCFRTSCursorState::MoveGround
			: ETCFRTSCursorState::Default;

	case ETCFRTSHoverTargetType::OtherActor:
		return ETCFRTSCursorState::InvalidTarget;

	case ETCFRTSHoverTargetType::None:
	default:
		return ETCFRTSCursorState::Default;
	}
}

ETCFSquadRelationship UTCFRTSHoverContextComponent::ResolveRelationshipToPrimarySelection(const AActor* HoveredActor) const
{
	if (!HoveredActor || !SelectionComponent || !RelationshipSubsystem)
	{
		return ETCFSquadRelationship::Neutral;
	}

	const ATCFSquadActor* PrimarySelectedSquad = SelectionComponent->GetPrimarySelectedSquad();
	if (!PrimarySelectedSquad)
	{
		return ETCFSquadRelationship::Neutral;
	}

	return RelationshipSubsystem->GetActorRelationship(PrimarySelectedSquad, HoveredActor);
}

bool UTCFRTSHoverContextComponent::AreHoverContextsEquivalent(
	const FTCFRTSHoverContext& A,
	const FTCFRTSHoverContext& B) const
{
	return A.bHasHit == B.bHasHit
		&& A.TargetType == B.TargetType
		&& A.CursorState == B.CursorState
		&& A.HoveredActor == B.HoveredActor
		&& A.RelationshipToPrimarySelection == B.RelationshipToPrimarySelection
		&& A.bHasPrimarySelection == B.bHasPrimarySelection;
}

void UTCFRTSHoverContextComponent::CreateCursorWidget()
{
	if (CursorWidget || !CursorWidgetClass || !PlayerController)
	{
		return;
	}

	CursorWidget = CreateWidget<UTCFRTSCursorWidget>(PlayerController, CursorWidgetClass);
	if (!CursorWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("RTS cursor widget class is not assigned."));
		return;
	}

	CursorWidget->InitializeCursorWidget(this);
	CursorWidget->AddToViewport(1000);

	PlayerController->CurrentMouseCursor = EMouseCursor::None;
}

