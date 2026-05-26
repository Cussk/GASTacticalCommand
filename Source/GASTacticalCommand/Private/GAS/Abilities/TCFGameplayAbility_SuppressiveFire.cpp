//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/Abilities/TCFGameplayAbility_SuppressiveFire.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFSquadActor.h"
#include "Data/TCFOrderDefinition.h"
#include "Subsystems/TCFSquadQuerySubsystem.h"

UTCFGameplayAbility_SuppressiveFire::UTCFGameplayAbility_SuppressiveFire()
{
	bCommitAbilityOnOrderActivated = true;
	bEndAbilityAfterOrderActivated = true;
}

void UTCFGameplayAbility_SuppressiveFire::HandleOrderActivated()
{
	FVector AreaLocation = FVector::ZeroVector;
	float AreaRadius = 0.0f;

	if (!TryGetSuppressionArea(AreaLocation, AreaRadius))
	{
		Super::HandleOrderActivated();
		return;
	}

	ATCFSquadActor* SourceSquad = GetSourceSquad();
	if (!SourceSquad)
	{
		Super::HandleOrderActivated();
		return;
	}

	UWorld* World = SourceSquad->GetWorld();
	if (!World)
	{
		Super::HandleOrderActivated();
		return;
	}

	UTCFSquadQuerySubsystem* SquadQuerySubsystem = World->GetSubsystem<UTCFSquadQuerySubsystem>();
	if (!SquadQuerySubsystem)
	{
		Super::HandleOrderActivated();
		return;
	}

	TArray<ATCFSquadActor*> TargetSquads;
	const FTCFSquadQueryRelationshipFilter RelationshipFilter = FTCFSquadQueryRelationshipFilter::EnemyOnly();
	SquadQuerySubsystem->GetSquadsInRadiusByRelationship(AreaLocation, AreaRadius, SourceSquad, RelationshipFilter, TargetSquads);

	ApplyGameplayEffectsToSelf(SourceFireEffects, SuppressiveFireEffectLevel, this);
	ApplyTargetSuppressionEffects(TargetSquads);

	Super::HandleOrderActivated();
}

bool UTCFGameplayAbility_SuppressiveFire::TryGetSuppressionArea(FVector& OutLocation, float& OutRadius) const
{
	const FTCFSquadOrderRequest Request = GetCurrentOrderRequest();
	const UTCFOrderDefinition* OrderDefinition = GetCurrentOrderDefinition();

	if (!OrderDefinition)
	{
		return false;
	}

	if (Request.Target.TargetType != ETCFOrderTargetType::Area
		&& Request.Target.TargetType != ETCFOrderTargetType::Location)
	{
		return false;
	}

	OutLocation = Request.Target.TargetLocation;

	OutRadius = Request.Target.Radius > 0.0f
		? Request.Target.Radius
		: OrderDefinition->Targeting.Radius;

	return OutRadius > 0.0f;
}

void UTCFGameplayAbility_SuppressiveFire::ApplyTargetSuppressionEffects(const TArray<ATCFSquadActor*>& Targets) const
{
	for (const ATCFSquadActor* TargetSquad : Targets)
	{
		if (!IsValid(TargetSquad))
		{
			continue;
		}

		UAbilitySystemComponent* TargetAbilitySystem = TargetSquad->GetAbilitySystemComponent();
		if (!TargetAbilitySystem)
		{
			continue;
		}

		ApplyGameplayEffectsToTarget(TargetAbilitySystem, TargetSuppressionEffects,	SuppressiveFireEffectLevel, const_cast<UTCFGameplayAbility_SuppressiveFire*>(this));
	}
}
