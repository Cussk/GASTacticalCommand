//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Debug/TCFDebugBridgeComponent.h"

#include "Debug/TCFDebugSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

UTCFDebugBridgeComponent::UTCFDebugBridgeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UTCFDebugBridgeComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		return;
	}

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UTCFDebugSubsystem* DebugSubsystem = LocalPlayer->GetSubsystem<UTCFDebugSubsystem>();
	if (!DebugSubsystem)
	{
		return;
	}

	DebugSubsystem->RegisterPlayerController(PlayerController, RefreshInterval);

	if (bCreateWidgetOnBeginPlay)
	{
		DebugSubsystem->ShowDebugHUD();
	}
}

void UTCFDebugBridgeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController)
	{
		if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UTCFDebugSubsystem* DebugSubsystem = LocalPlayer->GetSubsystem<UTCFDebugSubsystem>())
			{
				DebugSubsystem->UnregisterPlayerController(PlayerController);
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}
