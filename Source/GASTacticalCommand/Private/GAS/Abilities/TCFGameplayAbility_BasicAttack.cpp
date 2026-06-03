//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_BasicAttack.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFBuildingActor.h"
#include "Actors/TCFSquadActor.h"
#include "Data/TCFOrderDefinition.h"
#include "Subsystems/TCFRelationshipSubsystem.h"

UTCFGameplayAbility_BasicAttack::UTCFGameplayAbility_BasicAttack()
{
	bCommitAbilityOnOrderActivated = true;
	bEndAbilityAfterOrderActivated = true;
}

void UTCFGameplayAbility_BasicAttack::HandleOrderActivated()
{
	ATCFSquadActor* SourceSquad = GetSourceSquad();
	AActor* TargetActor = GetTargetActor();

	if (!IsValid(SourceSquad) || !IsValid(TargetActor))
	{
		return;
	}

	if (bRequireEnemyTarget && !IsValidEnemyTarget(*SourceSquad, *TargetActor))
	{
		return;
	}

	if (!IsTargetInRange(*SourceSquad, *TargetActor))
	{
		return;
	}

	if (ShouldRequireLineOfSight() && !HasLineOfSightToTarget(*SourceSquad, *TargetActor))
	{
		return;
	}

	UAbilitySystemComponent* TargetAbilitySystem = GetTargetAbilitySystemComponent(TargetActor);
	if (!TargetAbilitySystem)
	{
		return;
	}

	ApplyGameplayEffectsToTarget(
		TargetAbilitySystem,
		TargetAttackEffects,
		AttackEffectLevel,
		this);
}

AActor* UTCFGameplayAbility_BasicAttack::GetTargetActor() const
{
	if (CurrentOrderRequest.Target.TargetType != ETCFOrderTargetType::Actor)
	{
		return nullptr;
	}

	return CurrentOrderRequest.Target.TargetActor;
}

UAbilitySystemComponent* UTCFGameplayAbility_BasicAttack::GetTargetAbilitySystemComponent(
	AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		return nullptr;
	}

	if (const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(TargetActor))
	{
		return AbilitySystemInterface->GetAbilitySystemComponent();
	}

	return nullptr;
}
float UTCFGameplayAbility_BasicAttack::GetAttackRange() const
{
	if (CurrentOrderDefinition && CurrentOrderDefinition->Targeting.Range > 0.0f)
	{
		return CurrentOrderDefinition->Targeting.Range;
	}

	return FallbackAttackRange;
}

FVector UTCFGameplayAbility_BasicAttack::GetTargetRangeLocation(const AActor& TargetActor) const
{
	if (CurrentOrderRequest.Target.bUseTargetLocationForRange)
	{
		return CurrentOrderRequest.Target.TargetLocation;
	}

	if (const ATCFBuildingActor* Building = Cast<ATCFBuildingActor>(&TargetActor))
	{
		if (const UPrimitiveComponent* InteractionComponent = Building->GetInteractionCollisionComponent())
		{
			const FVector SourceLocation = GetSourceSquad()
				? GetSourceSquad()->GetActorLocation()
				: TargetActor.GetActorLocation();

			FVector ClosestPoint = InteractionComponent->Bounds.GetBox().GetClosestPointTo(SourceLocation);
			ClosestPoint.Z = SourceLocation.Z;
			return ClosestPoint;
		}
	}

	return TargetActor.GetActorLocation();
}

bool UTCFGameplayAbility_BasicAttack::IsTargetInRange(
	const ATCFSquadActor& SourceSquad,
	const AActor& TargetActor) const
{
	const float AttackRange = GetAttackRange();
	if (AttackRange <= 0.0f)
	{
		return true;
	}

	const float DistanceSquared = FVector::DistSquared2D(
		SourceSquad.GetActorLocation(),
		GetTargetRangeLocation(TargetActor));

	return DistanceSquared <= FMath::Square(AttackRange);
}

bool UTCFGameplayAbility_BasicAttack::ShouldRequireLineOfSight() const
{
	return bRequireLineOfSight
		|| (CurrentOrderDefinition && CurrentOrderDefinition->Targeting.bRequiresLineOfSight);
}

bool UTCFGameplayAbility_BasicAttack::HasLineOfSightToTarget(
	const ATCFSquadActor& SourceSquad,
	const AActor& TargetActor) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector StartLocation = SourceSquad.GetActorLocation();
	const FVector EndLocation = GetTargetRangeLocation(TargetActor);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TCFBasicAttackLineOfSight), false);
	QueryParams.AddIgnoredActor(&SourceSquad);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		LineOfSightTraceChannel,
		QueryParams);

	if (!bHit)
	{
		return true;
	}

	const AActor* HitActor = HitResult.GetActor();
	return HitActor == &TargetActor || HitActor->IsAttachedTo(&TargetActor);
}

bool UTCFGameplayAbility_BasicAttack::IsValidEnemyTarget(
	const ATCFSquadActor& SourceSquad,
	const AActor& TargetActor) const
{
	const UWorld* World = GetWorld();
	const UTCFRelationshipSubsystem* RelationshipSubsystem = World
		? World->GetSubsystem<UTCFRelationshipSubsystem>()
		: nullptr;

	if (!RelationshipSubsystem)
	{
		return false;
	}

	return RelationshipSubsystem->GetActorRelationship(&SourceSquad, &TargetActor) == ETCFSquadRelationship::Enemy;
}