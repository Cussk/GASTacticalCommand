// Copyright Kyle Cuss and Cuss Programming 2026.

#include "Player/TCFPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Components/TCFPlayerConstructionComponent.h"
#include "Components/TCFPlayerResourceBankComponent.h"

ATCFPlayerState::ATCFPlayerState()
{
	CommanderAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("CommanderAbilitySystemComponent"));
	CommanderAbilitySystemComponent->SetIsReplicated(true);
	CommanderAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PlayerResourceBankComponent = CreateDefaultSubobject<UTCFPlayerResourceBankComponent>(TEXT("PlayerResourceBankComponent"));
	PlayerConstructionComponent = CreateDefaultSubobject<UTCFPlayerConstructionComponent>(TEXT("PlayerConstructionComponent"));
}

void ATCFPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (CommanderAbilitySystemComponent)
	{
		CommanderAbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	if (HasAuthority() && PlayerConstructionComponent)
	{
		PlayerConstructionComponent->GrantConstructionAbility();
	}
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