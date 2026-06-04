// Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/TCFPlayerConstructionComponent.h"

#include "AbilitySystemComponent.h"
#include "Actors/TCFBuildingActor.h"
#include "Components/TCFAffiliationComponent.h"
#include "Components/TCFPlayerResourceBankComponent.h"
#include "Data/TCFBuildingDefinition.h"
#include "Data/TCFCommanderBuildCatalogDefinition.h"
#include "Data/TCFConstructionOptionDefinition.h"
#include "GAS/Abilities/TCFGameplayAbility_ConstructBuilding.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TCFPlayerState.h"
#include "Subsystems/TCFRTSPlacementGridSubsystem.h"

UTCFPlayerConstructionComponent::UTCFPlayerConstructionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ConstructBuildingAbilityClass = UTCFGameplayAbility_ConstructBuilding::StaticClass();
}

void UTCFPlayerConstructionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwner()->HasAuthority())
	{
		GrantConstructionAbility();
		GrantInitialCommanderTags();
	}
}

void UTCFPlayerConstructionComponent::GrantConstructionAbility()
{
	ATCFPlayerState* TCFPlayerState = GetTCFPlayerState();
	if (!TCFPlayerState || !TCFPlayerState->HasAuthority() || ConstructionAbilitySpecHandle.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* CommanderASC = GetCommanderAbilitySystemComponent();
	if (!CommanderASC || !ConstructBuildingAbilityClass)
	{
		return;
	}

	ConstructionAbilitySpecHandle = CommanderASC->GiveAbility(
		FGameplayAbilitySpec(ConstructBuildingAbilityClass, 1, INDEX_NONE, this));
}

bool UTCFPlayerConstructionComponent::HasPendingConstructionRequest() const
{
	return PendingConstructionRequest.IsValid();
}

const FTCFBuildingConstructionRequest& UTCFPlayerConstructionComponent::GetPendingConstructionRequest() const
{
	return PendingConstructionRequest;
}

void UTCFPlayerConstructionComponent::ClearPendingConstructionRequest()
{
	PendingConstructionRequest = FTCFBuildingConstructionRequest();
}

bool UTCFPlayerConstructionComponent::CanExecutePendingConstructionRequest(
	FTCFPlacementGridValidationResult* OutValidationResult) const
{
	if (!PendingConstructionRequest.IsValid())
	{
		if (OutValidationResult)
		{
			*OutValidationResult = FTCFPlacementGridValidationResult::Failure(
				ETCFPlacementGridValidationFailure::InvalidFootprint);
		}

		return false;
	}

	const UWorld* World = GetWorld();
	const UTCFRTSPlacementGridSubsystem* PlacementGrid = World
		? World->GetSubsystem<UTCFRTSPlacementGridSubsystem>()
		: nullptr;

	if (!PlacementGrid)
	{
		if (OutValidationResult)
		{
			*OutValidationResult = FTCFPlacementGridValidationResult::Failure(
				ETCFPlacementGridValidationFailure::InvalidFootprint);
		}

		return false;
	}

	FTCFPlacementGridValidationResult ValidationResult;
	const bool bValid = const_cast<UTCFRTSPlacementGridSubsystem*>(PlacementGrid)->ValidateFootprint(
		PendingConstructionRequest.AnchorCell,
		PendingConstructionRequest.BuildingDefinition->GetSafeFootprintSize(),
		nullptr,
		ValidationResult);

	if (OutValidationResult)
	{
		*OutValidationResult = ValidationResult;
	}

	return bValid;
}

bool UTCFPlayerConstructionComponent::ExecutePendingConstruction(ATCFBuildingActor*& OutPlacedBuilding)
{
	OutPlacedBuilding = nullptr;

	FTCFPlacementGridValidationResult ValidationResult;
	if (!CanExecutePendingConstructionRequest(&ValidationResult))
	{
		return false;
	}

	UTCFBuildingDefinition* BuildingDefinition = PendingConstructionRequest.BuildingDefinition;
	if (!BuildingDefinition || !GetWorld())
	{
		return false;
	}

	const TSubclassOf<ATCFBuildingActor> BuildingClass = BuildingDefinition->GetBuildingActorClass();
	if (!BuildingClass)
	{
		return false;
	}

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(PendingConstructionRequest.PlacementLocation);
	SpawnTransform.SetRotation(FQuat::Identity);
	SpawnTransform.SetScale3D(FVector::OneVector);

	ATCFBuildingActor* PlacedBuilding = GetWorld()->SpawnActorDeferred<ATCFBuildingActor>(
		BuildingClass,
		SpawnTransform,
		GetOwner(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!PlacedBuilding)
	{
		return false;
	}

	PlacedBuilding->ApplyBuildingDefinition(BuildingDefinition, true);
	ApplyPlacedBuildingAffiliation(PlacedBuilding);

	UGameplayStatics::FinishSpawningActor(PlacedBuilding, SpawnTransform);

	if (BuildingDefinition->bBlocksBuildingPlacement && !PlacedBuilding->HasReservedPlacementFootprint())
	{
		PlacedBuilding->Destroy();
		return false;
	}

	LastPlacedBuilding = PlacedBuilding;
	OutPlacedBuilding = PlacedBuilding;

	OnConstructionSitePlaced.Broadcast(
		PlacedBuilding,
		BuildingDefinition,
		PendingConstructionRequest.PlacementLocation,
		PendingConstructionRequest.AnchorCell);

	return true;
}

void UTCFPlayerConstructionComponent::RefundPendingConstructionCost() const
{
	const UTCFConstructionOptionDefinition* ConstructionOptionDefinition = PendingConstructionRequest.ConstructionOption;
	if (!ConstructionOptionDefinition)
	{
		return;
	}

	UTCFPlayerResourceBankComponent* ResourceBank = GetResourceBankComponent();
	if (!ResourceBank)
	{
		return;
	}

	ResourceBank->AddResources(ConstructionOptionDefinition->GetEffectiveCost());
}

ATCFBuildingActor* UTCFPlayerConstructionComponent::GetLastPlacedBuilding() const
{
	return LastPlacedBuilding;
}

void UTCFPlayerConstructionComponent::GetVisibleConstructionOptions(
	TArray<UTCFConstructionOptionDefinition*>& OutOptions) const
{
	OutOptions.Reset();

	if (!BuildCatalog)
	{
		return;
	}

	TArray<UTCFConstructionOptionDefinition*> CatalogOptions;
	BuildCatalog->GetConstructionOptions(CatalogOptions);

	for (UTCFConstructionOptionDefinition* Option : CatalogOptions)
	{
		if (!IsValid(Option))
		{
			continue;
		}

		FTCFConstructionAccessResult AccessResult;
		const bool bCanAccess = CanAccessConstructionOption(Option, AccessResult);

		if (bCanAccess || Option->bShowWhenLocked)
		{
			OutOptions.Add(Option);
		}
	}

	OutOptions.Sort([](
		const UTCFConstructionOptionDefinition& Left,
		const UTCFConstructionOptionDefinition& Right)
	{
		return Left.SortPriority < Right.SortPriority;
	});
}

bool UTCFPlayerConstructionComponent::CanAccessConstructionOption(
	UTCFConstructionOptionDefinition* ConstructionOption,
	FTCFConstructionAccessResult& OutResult) const
{
	OutResult = FTCFConstructionAccessResult();

	if (!IsValid(ConstructionOption))
	{
		return false;
	}

	OutResult.bHasOption = true;

	UTCFBuildingDefinition* BuildingDefinition = ConstructionOption->GetBuildingDefinition();
	if (!IsValid(BuildingDefinition))
	{
		return false;
	}

	OutResult.bHasBuildingDefinition = true;

	const UAbilitySystemComponent* CommanderASC = GetCommanderAbilitySystemComponent();
	FGameplayTagContainer OwnedTags;

	if (CommanderASC)
	{
		CommanderASC->GetOwnedGameplayTags(OwnedTags);
	}

	for (const FGameplayTag& RequiredTag : ConstructionOption->RequiredCommanderTags)
	{
		if (RequiredTag.IsValid() && !OwnedTags.HasTagExact(RequiredTag))
		{
			OutResult.MissingRequiredTags.AddTag(RequiredTag);
		}
	}

	for (const FGameplayTag& BlockedTag : ConstructionOption->BlockedCommanderTags)
	{
		if (BlockedTag.IsValid() && OwnedTags.HasTagExact(BlockedTag))
		{
			OutResult.MatchingBlockedTags.AddTag(BlockedTag);
		}
	}

	OutResult.bMeetsRequiredTags = OutResult.MissingRequiredTags.IsEmpty();
	OutResult.bBlockedByTags = !OutResult.MatchingBlockedTags.IsEmpty();
	OutResult.bCanAccess = OutResult.bMeetsRequiredTags && !OutResult.bBlockedByTags;

	return OutResult.bCanAccess;
}

bool UTCFPlayerConstructionComponent::TryRequestBuildingConstructionOption(
	UTCFConstructionOptionDefinition* ConstructionOption,
	FVector PlacementLocation,
	FIntPoint AnchorCell,
	const FTCFPlacementGridValidationResult& PlacementValidationResult,
	ATCFBuildingActor*& OutPlacedBuilding)
{
	OutPlacedBuilding = nullptr;
	LastPlacedBuilding = nullptr;

	FTCFConstructionAccessResult AccessResult;
	if (!CanAccessConstructionOption(ConstructionOption, AccessResult))
	{
		return false;
	}

	UTCFBuildingDefinition* BuildingDefinition = ConstructionOption->GetBuildingDefinition();
	if (!IsValid(BuildingDefinition) || !PlacementValidationResult.bIsValid)
	{
		return false;
	}

	PendingConstructionRequest.ConstructionOption = ConstructionOption;
	PendingConstructionRequest.BuildingDefinition = BuildingDefinition;
	PendingConstructionRequest.PlacementLocation = PlacementLocation;
	PendingConstructionRequest.AnchorCell = AnchorCell;
	PendingConstructionRequest.PlacementValidationResult = PlacementValidationResult;

	if (!CanExecutePendingConstructionRequest())
	{
		ClearPendingConstructionRequest();
		return false;
	}

	UAbilitySystemComponent* CommanderASC = GetCommanderAbilitySystemComponent();
	if (!CommanderASC || !ConstructBuildingAbilityClass)
	{
		ClearPendingConstructionRequest();
		return false;
	}

	const bool bActivationStarted = CommanderASC->TryActivateAbilityByClass(ConstructBuildingAbilityClass);

	OutPlacedBuilding = LastPlacedBuilding;
	const bool bSucceeded = bActivationStarted && IsValid(OutPlacedBuilding);

	if (!bSucceeded)
	{
		ClearPendingConstructionRequest();
	}

	return bSucceeded;
}

UTCFPlayerResourceBankComponent* UTCFPlayerConstructionComponent::GetResourceBankComponent() const
{
	const ATCFPlayerState* TCFPlayerState = GetTCFPlayerState();
	return TCFPlayerState
		? TCFPlayerState->GetPlayerResourceBankComponent()
		: nullptr;
}

void UTCFPlayerConstructionComponent::GrantInitialCommanderTags()
{
	UAbilitySystemComponent* CommanderASC = GetCommanderAbilitySystemComponent();
	if (!CommanderASC)
	{
		return;
	}

	for (const FGameplayTag& Tag : InitialCommanderTags)
	{
		if (Tag.IsValid())
		{
			CommanderASC->AddLooseGameplayTag(Tag);
		}
	}
}

ATCFPlayerState* UTCFPlayerConstructionComponent::GetTCFPlayerState() const
{
	return Cast<ATCFPlayerState>(GetOwner());
}

UAbilitySystemComponent* UTCFPlayerConstructionComponent::GetCommanderAbilitySystemComponent() const
{
	const ATCFPlayerState* TCFPlayerState = GetTCFPlayerState();
	return TCFPlayerState
		? TCFPlayerState->GetAbilitySystemComponent()
		: nullptr;
}

void UTCFPlayerConstructionComponent::ApplyPlacedBuildingAffiliation(ATCFBuildingActor* PlacedBuilding) const
{
	if (!PlacedBuilding)
	{
		return;
	}

	const ATCFPlayerState* TCFPlayerState = GetTCFPlayerState();
	if (!TCFPlayerState)
	{
		return;
	}

	const UTCFAffiliationComponent* PlayerAffiliation = TCFPlayerState->GetAffiliationComponent();
	UTCFAffiliationComponent* BuildingAffiliation = PlacedBuilding->GetAffiliationComponent();

	if (!PlayerAffiliation || !BuildingAffiliation)
	{
		return;
	}

	BuildingAffiliation->SetAffiliation(PlayerAffiliation->GetAffiliation());
}