//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TCFAffiliationTypes.generated.h"

UENUM(BlueprintType)
enum class ETCFSquadRelationship : uint8
{
	Own,
	Friendly,
	Neutral,
	Enemy
};

USTRUCT(BlueprintType)
struct FTCFAffiliationData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Affiliation")
	int32 OwnerId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Affiliation")
	int32 TeamId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Affiliation")
	FGameplayTag FactionTag;

	bool HasOwner() const
	{
		return OwnerId > 0;
	}

	bool HasTeam() const
	{
		return TeamId > 0;
	}

	bool HasFaction() const
	{
		return FactionTag.IsValid();
	}
};

USTRUCT(BlueprintType)
struct FTCFSquadQueryRelationshipFilter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Query")
	bool bIncludeOwn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Query")
	bool bIncludeFriendly = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Query")
	bool bIncludeNeutral = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TCF|Query")
	bool bIncludeEnemy = true;

	bool AllowsRelationship(ETCFSquadRelationship Relationship) const
	{
		switch (Relationship)
		{
		case ETCFSquadRelationship::Own:
			return bIncludeOwn;

		case ETCFSquadRelationship::Friendly:
			return bIncludeFriendly;

		case ETCFSquadRelationship::Neutral:
			return bIncludeNeutral;

		case ETCFSquadRelationship::Enemy:
			return bIncludeEnemy;

		default:
			return false;
		}
	}

	static FTCFSquadQueryRelationshipFilter EnemyOnly()
	{
		FTCFSquadQueryRelationshipFilter Filter;
		Filter.bIncludeOwn = false;
		Filter.bIncludeFriendly = false;
		Filter.bIncludeNeutral = false;
		Filter.bIncludeEnemy = true;
		return Filter;
	}

	static FTCFSquadQueryRelationshipFilter OwnAndFriendly()
	{
		FTCFSquadQueryRelationshipFilter Filter;
		Filter.bIncludeOwn = true;
		Filter.bIncludeFriendly = true;
		Filter.bIncludeNeutral = false;
		Filter.bIncludeEnemy = false;
		return Filter;
	}

	static FTCFSquadQueryRelationshipFilter All()
	{
		FTCFSquadQueryRelationshipFilter Filter;
		Filter.bIncludeOwn = true;
		Filter.bIncludeFriendly = true;
		Filter.bIncludeNeutral = true;
		Filter.bIncludeEnemy = true;
		return Filter;
	}
};
