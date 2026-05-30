// Copyright Kyle Cuss and Cuss Programming 2026.

#include "Player/TCFPlayerState.h"
#include "Components/TCFPlayerResourceBankComponent.h"

ATCFPlayerState::ATCFPlayerState()
{
	PlayerResourceBankComponent = CreateDefaultSubobject<UTCFPlayerResourceBankComponent>(TEXT("PlayerResourceBankComponent"));
}

UTCFPlayerResourceBankComponent* ATCFPlayerState::GetPlayerResourceBankComponent() const
{
	return PlayerResourceBankComponent;
}
