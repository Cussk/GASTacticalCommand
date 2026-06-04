// Copyright Kyle Cuss and Cuss Programming 2026.

#include "Player/TCFPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Components/TCFAffiliationComponent.h"
#include "Components/TCFPlayerConstructionComponent.h"
#include "Components/TCFPlayerResourceBankComponent.h"

ATCFPlayerState::ATCFPlayerState()
{
	CommanderAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("CommanderAbilitySystemComponent"));
	CommanderAbilitySystemComponent->SetIsReplicated(true);
	CommanderAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PlayerResourceBankComponent = CreateDefaultSubobject<UTCFPlayerResourceBankComponent>(TEXT("PlayerResourceBankComponent"));
	PlayerConstructionComponent = CreateDefaultSubobject<UTCFPlayerConstructionComponent>(TEXT("PlayerConstructionComponent"));
	AffiliationComponent = CreateDefaultSubobject<UTCFAffiliationComponent>(TEXT("AffiliationComponent"));
}

void ATCFPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (CommanderAbilitySystemComponent)
	{
		CommanderAbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	
	InitializePlayerAffiliation();
}

UAbilitySystemComponent* ATCFPlayerState::GetAbilitySystemComponent() const
{
	return CommanderAbilitySystemComponent;
}

UTCFPlayerResourceBankComponent* ATCFPlayerState::GetPlayerResourceBankComponent() const
{
	return PlayerResourceBankComponent;
}

UTCFPlayerConstructionComponent* ATCFPlayerState::GetPlayerConstructionComponent() const
{
	return PlayerConstructionComponent;
}

UTCFAffiliationComponent* ATCFPlayerState::GetAffiliationComponent() const
{
	return AffiliationComponent;
}

int32 ATCFPlayerState::GetTeamId() const
{
	return AffiliationComponent ? AffiliationComponent->GetAffiliation().TeamId : InitialTeamId;
}

void ATCFPlayerState::InitializePlayerAffiliation() const
{
	if (!AffiliationComponent)
	{
		return;
	}

	FTCFAffiliationData Affiliation;
	Affiliation.OwnerId = FMath::Max(0, GetPlayerId());
	Affiliation.TeamId = FMath::Max(1, InitialTeamId);
	Affiliation.FactionTag = InitialFactionTag;

	AffiliationComponent->SetAffiliation(Affiliation);
}
