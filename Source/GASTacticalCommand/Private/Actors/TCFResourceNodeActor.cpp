//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Actors/TCFResourceNodeActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/TCFResourceNodeDefinition.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

ATCFResourceNodeActor::ATCFResourceNodeActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	NodeVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NodeVisual"));
	NodeVisual->SetupAttachment(SceneRoot);
	NodeVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NodeVisual->SetGenerateOverlapEvents(false);

	InteractionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(SceneRoot);
	InteractionCollision->SetSphereRadius(160.0f);
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InteractionCollision->SetGenerateOverlapEvents(false);
}

void ATCFResourceNodeActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyDefinitionVisuals();
}

void ATCFResourceNodeActor::BeginPlay()
{
	Super::BeginPlay();

	ApplyDefinitionVisuals();

	if (HasAuthority())
	{
		InitializeResourceNode();
	}
}

void ATCFResourceNodeActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATCFResourceNodeActor, NodeDefinition);
	DOREPLIFETIME(ATCFResourceNodeActor, CurrentAmount);
	DOREPLIFETIME(ATCFResourceNodeActor, MaxAmount);
	DOREPLIFETIME(ATCFResourceNodeActor, bDepleted);
}

void ATCFResourceNodeActor::ApplyResourceNodeDefinition(
	UTCFResourceNodeDefinition* NewDefinition,
	bool bResetAmount)
{
	if (!HasAuthority())
	{
		return;
	}

	NodeDefinition = NewDefinition;
	ApplyDefinitionVisuals();

	if (bResetAmount)
	{
		InitializeResourceNode();
	}
}

void ATCFResourceNodeActor::InitializeResourceNode()
{
	if (!HasAuthority() || !NodeDefinition)
	{
		return;
	}

	const int32 InitialAmount = NodeDefinition->RollInitialAmount();

	MaxAmount = InitialAmount;
	bDepleted = false;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SetCurrentAmount(InitialAmount);
}

UTCFResourceNodeDefinition* ATCFResourceNodeActor::GetResourceNodeDefinition() const
{
	return NodeDefinition;
}

FGameplayTag ATCFResourceNodeActor::GetResourceType() const
{
	return NodeDefinition ? NodeDefinition->ResourceType : FGameplayTag();
}

int32 ATCFResourceNodeActor::GetCurrentAmount() const
{
	return CurrentAmount;
}

int32 ATCFResourceNodeActor::GetMaxAmount() const
{
	return MaxAmount;
}

bool ATCFResourceNodeActor::IsDepleted() const
{
	return bDepleted;
}

bool ATCFResourceNodeActor::CanGatherResource() const
{
	return NodeDefinition
		&& NodeDefinition->ResourceType.IsValid()
		&& !bDepleted
		&& CurrentAmount > 0;
}

bool ATCFResourceNodeActor::TryGatherResource(
	int32 RequestedAmount,
	FTCFResourceAmount& OutGatheredResource)
{
	OutGatheredResource = FTCFResourceAmount();

	if (!HasAuthority() || !CanGatherResource())
	{
		return false;
	}

	const int32 SafeRequestedAmount = RequestedAmount > 0
		? RequestedAmount
		: NodeDefinition->GetSafeGatherAmount();

	const int32 GatheredAmount = FMath::Min(CurrentAmount, SafeRequestedAmount);
	if (GatheredAmount <= 0)
	{
		return false;
	}

	OutGatheredResource.ResourceType = NodeDefinition->ResourceType;
	OutGatheredResource.Amount = GatheredAmount;

	SetCurrentAmount(CurrentAmount - GatheredAmount);
	return true;
}

void ATCFResourceNodeActor::SetCurrentAmount(int32 NewAmount)
{
	if (!HasAuthority())
	{
		return;
	}

	const int32 OldAmount = CurrentAmount;
	CurrentAmount = FMath::Clamp(NewAmount, 0, FMath::Max(0, MaxAmount));

	if (OldAmount != CurrentAmount)
	{
		OnResourceAmountChanged.Broadcast(this, OldAmount, CurrentAmount);
	}

	if (CurrentAmount <= 0)
	{
		HandleDepleted();
	}
	else if (bDepleted)
	{
		RestoreFromDepleted();
	}
}

void ATCFResourceNodeActor::OnRep_NodeDefinition() const
{
	ApplyDefinitionVisuals();
}

void ATCFResourceNodeActor::OnRep_CurrentAmount()
{
	if (CurrentAmount <= 0)
	{
		ApplyDepletedPresentation();
	}
}

void ATCFResourceNodeActor::OnRep_Depleted()
{
	ApplyDepletedPresentation();
}

void ATCFResourceNodeActor::ApplyDefinitionVisuals() const
{
	if (!NodeDefinition)
	{
		return;
	}

	if (NodeDefinition->VisualMesh)
	{
		NodeVisual->SetStaticMesh(NodeDefinition->VisualMesh);
	}

	if (NodeDefinition->VisualMaterial)
	{
		NodeVisual->SetMaterial(0, NodeDefinition->VisualMaterial);
	}

	NodeVisual->SetRelativeScale3D(NodeDefinition->VisualScale);
}

void ATCFResourceNodeActor::HandleDepleted()
{
	if (bDepleted)
	{
		return;
	}

	bDepleted = true;
	OnResourceNodeDepleted.Broadcast(this);
	ApplyDepletedPresentation();

	if (NodeDefinition
		&& NodeDefinition->DepletionBehavior == ETCFResourceNodeDepletionBehavior::Destroy)
	{
		ScheduleDestroyAfterDepletion();
	}
}

void ATCFResourceNodeActor::RestoreFromDepleted()
{
	bDepleted = false;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NodeVisual->SetVisibility(true, true);
}

void ATCFResourceNodeActor::ApplyDepletedPresentation()
{
	if (!bDepleted || !NodeDefinition)
	{
		return;
	}

	switch (NodeDefinition->DepletionBehavior)
	{
	case ETCFResourceNodeDepletionBehavior::StayEmpty:
		// Could change mesh or material to show as depleted
		break;

	case ETCFResourceNodeDepletionBehavior::Hide:
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		InteractionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case ETCFResourceNodeDepletionBehavior::Destroy:
		break;

	default:
		break;
	}
}

void ATCFResourceNodeActor::ScheduleDestroyAfterDepletion()
{
	if (!HasAuthority())
	{
		return;
	}

	const float DelaySeconds = NodeDefinition
		? NodeDefinition->DestroyDelaySeconds
		: 0.0f;

	if (DelaySeconds <= 0.0f)
	{
		Destroy();
		return;
	}

	FTimerHandle DestroyTimerHandle;
	GetWorldTimerManager().SetTimer(
		DestroyTimerHandle,
		this,
		&ATCFResourceNodeActor::DestroyNode,
		DelaySeconds,
		false);
}

void ATCFResourceNodeActor::DestroyNode()
{
	Destroy();
}