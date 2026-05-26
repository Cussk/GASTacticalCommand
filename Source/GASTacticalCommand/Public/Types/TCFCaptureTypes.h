//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/TCFAffiliationTypes.h"
#include "TCFCaptureTypes.generated.h"

UENUM(BlueprintType)
enum class ETCFCapturePointState : uint8
{
	Neutral,
	Owned,
	Capturing,
	Contested
};

UENUM(BlueprintType)
enum class ETCFCaptureSideType : uint8
{
	None,
	Owner,
	Team,
	Faction
};

UENUM(BlueprintType)
enum class ETCFCaptureOwnershipPolicy : uint8
{
	Team,
	Owner,
	Faction
};

USTRUCT(BlueprintType)
struct FTCFCaptureSideKey
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Capture")
	ETCFCaptureSideType SideType = ETCFCaptureSideType::None;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Capture")
	int32 NumericId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "TCF|Capture")
	FGameplayTag FactionTag;

	bool IsValid() const
	{
		switch (SideType)
		{
		case ETCFCaptureSideType::Owner:
		case ETCFCaptureSideType::Team:
			return NumericId > 0;

		case ETCFCaptureSideType::Faction:
			return FactionTag.IsValid();

		default:
			return false;
		}
	}

	bool operator==(const FTCFCaptureSideKey& Other) const
	{
		if (SideType != Other.SideType)
		{
			return false;
		}

		switch (SideType)
		{
		case ETCFCaptureSideType::Owner:
		case ETCFCaptureSideType::Team:
			return NumericId == Other.NumericId;

		case ETCFCaptureSideType::Faction:
			return FactionTag == Other.FactionTag;

		case ETCFCaptureSideType::None:
		default:
			return true;
		}
	}

	bool operator!=(const FTCFCaptureSideKey& Other) const
	{
		return !(*this == Other);
	}

	FString ToDebugString() const
	{
		switch (SideType)
		{
		case ETCFCaptureSideType::Owner:
			return FString::Printf(TEXT("Owner:%d"), NumericId);

		case ETCFCaptureSideType::Team:
			return FString::Printf(TEXT("Team:%d"), NumericId);

		case ETCFCaptureSideType::Faction:
			return FString::Printf(TEXT("Faction:%s"), *FactionTag.ToString());

		default:
			return TEXT("None");
		}
	}

	static FTCFCaptureSideKey FromAffiliation(
		const FTCFAffiliationData& Affiliation,
		ETCFCaptureOwnershipPolicy OwnershipPolicy = ETCFCaptureOwnershipPolicy::Team)
	{
		FTCFCaptureSideKey Key;

		switch (OwnershipPolicy)
		{
		case ETCFCaptureOwnershipPolicy::Team:
			if (Affiliation.HasTeam())
			{
				Key.SideType = ETCFCaptureSideType::Team;
				Key.NumericId = Affiliation.TeamId;
				return Key;
			}

			if (Affiliation.HasOwner())
			{
				Key.SideType = ETCFCaptureSideType::Owner;
				Key.NumericId = Affiliation.OwnerId;
				return Key;
			}

			if (Affiliation.HasFaction())
			{
				Key.SideType = ETCFCaptureSideType::Faction;
				Key.FactionTag = Affiliation.FactionTag;
				return Key;
			}

			return Key;

		case ETCFCaptureOwnershipPolicy::Owner:
			if (Affiliation.HasOwner())
			{
				Key.SideType = ETCFCaptureSideType::Owner;
				Key.NumericId = Affiliation.OwnerId;
				return Key;
			}

			if (Affiliation.HasTeam())
			{
				Key.SideType = ETCFCaptureSideType::Team;
				Key.NumericId = Affiliation.TeamId;
				return Key;
			}

			if (Affiliation.HasFaction())
			{
				Key.SideType = ETCFCaptureSideType::Faction;
				Key.FactionTag = Affiliation.FactionTag;
				return Key;
			}

			return Key;

		case ETCFCaptureOwnershipPolicy::Faction:
			if (Affiliation.HasFaction())
			{
				Key.SideType = ETCFCaptureSideType::Faction;
				Key.FactionTag = Affiliation.FactionTag;
				return Key;
			}

			if (Affiliation.HasTeam())
			{
				Key.SideType = ETCFCaptureSideType::Team;
				Key.NumericId = Affiliation.TeamId;
				return Key;
			}

			if (Affiliation.HasOwner())
			{
				Key.SideType = ETCFCaptureSideType::Owner;
				Key.NumericId = Affiliation.OwnerId;
				return Key;
			}

			return Key;

		default:
			return Key;
		}
	}
};

FORCEINLINE uint32 GetTypeHash(const FTCFCaptureSideKey& Key)
{
	const uint32 Hash = GetTypeHash(static_cast<uint8>(Key.SideType));

	switch (Key.SideType)
	{
	case ETCFCaptureSideType::Owner:
	case ETCFCaptureSideType::Team:
		return HashCombine(Hash, GetTypeHash(Key.NumericId));

	case ETCFCaptureSideType::Faction:
		return HashCombine(Hash, GetTypeHash(Key.FactionTag));

	case ETCFCaptureSideType::None:
	default:
		return Hash;
	}
}

USTRUCT()
struct FTCFCaptureSideScore
{
	GENERATED_BODY()

	FTCFCaptureSideKey SideKey;
	FTCFAffiliationData RepresentativeAffiliation;

	float TotalCapturePower = 0.0f;
	int32 SquadCount = 0;
};
