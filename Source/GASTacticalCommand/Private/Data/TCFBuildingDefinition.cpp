//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Data/TCFBuildingDefinition.h"

#include "Actors/TCFBuildingActor.h"

FIntPoint UTCFBuildingDefinition::GetSafeFootprintSize() const
{
	return FIntPoint(
		FMath::Max(1, FootprintSize.X),
		FMath::Max(1, FootprintSize.Y));
}

TSubclassOf<ATCFBuildingActor> UTCFBuildingDefinition::GetBuildingActorClass() const
{
	if (BuildingActorClass)
	{
		return BuildingActorClass;
	} 
	
	return ATCFBuildingActor::StaticClass();
}