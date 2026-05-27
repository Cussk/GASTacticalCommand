//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_BasicAttack.h"

#include "AbilitySystemComponent.h"
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
	ATCFSquadActor* TargetSquad = GetTargetSquad();

	if (!IsValid(SourceSquad) || !IsValid(TargetSquad))
	{
		return;
	}

	if (bRequireEnemyTarget && !IsValidEnemyTarget(*SourceSquad, *TargetSquad))
	{
		return;
	}

	if (!IsTargetInRange(*SourceSquad, *TargetSquad))
	{
		return;
	}

	if (ShouldRequireLineOfSight() && !HasLineOfSightToTarget(*SourceSquad, *TargetSquad))
	{
		return;
	}

	UAbilitySystemComponent* TargetAbilitySystem = TargetSquad->GetAbilitySystemComponent();
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

ATCFSquadActor* UTCFGameplayAbility_BasicAttack::GetTargetSquad() const
{
	if (CurrentOrderRequest.Target.TargetType != ETCFOrderTargetType::Actor)
	{
		return nullptr;
	}

	return Cast<ATCFSquadActor>(CurrentOrderRequest.Target.TargetActor);
}

float UTCFGameplayAbility_BasicAttack::GetAttackRange() const
{
	if (CurrentOrderDefinition && CurrentOrderDefinition->Targeting.Range > 0.0f)
	{
		return CurrentOrderDefinition->Targeting.Range;
	}

	return FallbackAttackRange;
}

bool UTCFGameplayAbility_BasicAttack::IsTargetInRange(
	const ATCFSquadActor& SourceSquad,
	const ATCFSquadActor& TargetSquad) const
{
	const float AttackRange = GetAttackRange();
	if (AttackRange <= 0.0f)
	{
		return true;
	}

	const float DistanceSquared = FVector::DistSquared(
		SourceSquad.GetActorLocation(),
		TargetSquad.GetActorLocation());

	return DistanceSquared <= FMath::Square(AttackRange);
}

bool UTCFGameplayAbility_BasicAttack::ShouldRequireLineOfSight() const
{
	return bRequireLineOfSight
		|| (CurrentOrderDefinition && CurrentOrderDefinition->Targeting.bRequiresLineOfSight);
}

bool UTCFGameplayAbility_BasicAttack::HasLineOfSightToTarget(
	const ATCFSquadActor& SourceSquad,
	const ATCFSquadActor& TargetSquad) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector StartLocation = SourceSquad.GetActorLocation();
	const FVector EndLocation = TargetSquad.GetActorLocation();

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
	return HitActor == &TargetSquad || HitActor->IsAttachedTo(&TargetSquad);
}

bool UTCFGameplayAbility_BasicAttack::IsValidEnemyTarget(
	const ATCFSquadActor& SourceSquad,
	const ATCFSquadActor& TargetSquad) const
{
	const UWorld* World = GetWorld();
	const UTCFRelationshipSubsystem* RelationshipSubsystem = World
		? World->GetSubsystem<UTCFRelationshipSubsystem>()
		: nullptr;

	if (!RelationshipSubsystem)
	{
		return false;
	}

	return RelationshipSubsystem->GetActorRelationship(&SourceSquad, &TargetSquad) == ETCFSquadRelationship::Enemy;
}