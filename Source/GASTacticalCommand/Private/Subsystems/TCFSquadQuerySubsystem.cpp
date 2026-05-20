//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Subsystems/TCFSquadQuerySubsystem.h"

#include "Actors/TCFSquadActor.h"
#include "Subsystems/TCFRelationshipSubsystem.h"

void UTCFSquadQuerySubsystem::RegisterSquad(ATCFSquadActor* Squad)
{
	if (!IsValid(Squad))
	{
		return;
	}

	RegisteredSquads.Add(Squad);
}

void UTCFSquadQuerySubsystem::UnregisterSquad(ATCFSquadActor* Squad)
{
	if (!Squad)
	{
		return;
	}

	RegisteredSquads.Remove(Squad);
}

void UTCFSquadQuerySubsystem::GetSquadsInRadius(
	FVector Origin,
	float Radius,
	const ATCFSquadActor* IgnoredSquad,
	TArray<ATCFSquadActor*>& OutSquads)
{
	OutSquads.Reset();

	if (Radius <= 0.0f)
	{
		return;
	}

	CompactInvalidSquads();

	const float RadiusSquared = FMath::Square(Radius);

	for (const TWeakObjectPtr<ATCFSquadActor>& SquadPtr : RegisteredSquads)
	{
		ATCFSquadActor* Squad = SquadPtr.Get();
		if (!IsValid(Squad) || Squad == IgnoredSquad)
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(Origin, Squad->GetActorLocation());
		if (DistanceSquared <= RadiusSquared)
		{
			OutSquads.Add(Squad);
		}
	}
}

void UTCFSquadQuerySubsystem::GetSquadsInRadiusByRelationship(
	FVector Origin,
	float Radius,
	const AActor* SourceActor,
	const FTCFSquadQueryRelationshipFilter& RelationshipFilter,
	TArray<ATCFSquadActor*>& OutSquads)
{
	OutSquads.Reset();

	if (!IsValid(SourceActor) || Radius <= 0.0f)
	{
		return;
	}

	const UWorld* World = GetWorld();
	const UTCFRelationshipSubsystem* RelationshipSubsystem = World
		? World->GetSubsystem<UTCFRelationshipSubsystem>()
		: nullptr;

	if (!RelationshipSubsystem)
	{
		return;
	}

	CompactInvalidSquads();

	const float RadiusSquared = FMath::Square(Radius);

	for (const TWeakObjectPtr<ATCFSquadActor>& SquadPtr : RegisteredSquads)
	{
		ATCFSquadActor* Squad = SquadPtr.Get();
		if (!IsValid(Squad))
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(Origin, Squad->GetActorLocation());
		if (DistanceSquared > RadiusSquared)
		{
			continue;
		}

		const ETCFSquadRelationship Relationship = RelationshipSubsystem->GetActorRelationship(SourceActor, Squad);
		if (!RelationshipFilter.AllowsRelationship(Relationship))
		{
			continue;
		}

		OutSquads.Add(Squad);
	}
}

void UTCFSquadQuerySubsystem::CompactInvalidSquads()
{
	for (auto It = RegisteredSquads.CreateIterator(); It; ++It)
	{
		if (!It->IsValid())
		{
			It.RemoveCurrent();
		}
	}
}