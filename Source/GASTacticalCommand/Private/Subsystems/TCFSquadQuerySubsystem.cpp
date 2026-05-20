//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Subsystems/TCFSquadQuerySubsystem.h"

#include "Actors/TCFSquadActor.h"

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