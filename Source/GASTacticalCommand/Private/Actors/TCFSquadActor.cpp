//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Actors/TCFSquadActor.h"

#include "AbilitySystemComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/TCFSquadDefinition.h"
#include "GAS/TCFSquadAttributeSet.h"
#include "GameplayEffect.h"
#include "Components/TCFAffiliationComponent.h"
#include "Components/TCFSquadAttackCommandComponent.h"
#include "Components/TCFSquadIntegrityStateComponent.h"
#include "Components/TCFSquadMovementComponent.h"
#include "Components/TCFSquadSelectionComponent.h"
#include "GASTacticalCommand/GASTacticalCommand.h"
#include "Subsystems/TCFSquadQuerySubsystem.h"

ATCFSquadActor::ATCFSquadActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	SquadVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SquadVisual"));
	SquadVisual->SetupAttachment(SceneRoot);
	SquadVisual->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SquadVisual->SetCollisionResponseToAllChannels(ECR_Ignore);
	SquadVisual->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	MovementComponent = CreateDefaultSubobject<UTCFSquadMovementComponent>(TEXT("MovementComponent"));
	AttackCommandComponent = CreateDefaultSubobject<UTCFSquadAttackCommandComponent>(TEXT("AttackCommandComponent"));

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	SquadAttributeSet = CreateDefaultSubobject<UTCFSquadAttributeSet>(TEXT("SquadAttributeSet"));
	
	SquadSelectionComponent = CreateDefaultSubobject<UTCFSquadSelectionComponent>(TEXT("SquadSelectionComponent"));
	IntegrityStateComponent = CreateDefaultSubobject<UTCFSquadIntegrityStateComponent>(TEXT("IntegrityStateComponent"));
	AffiliationComponent = CreateDefaultSubobject<UTCFAffiliationComponent>(TEXT("AffiliationComponent"));
	
	SquadVisual->SetCustomDepthStencilValue(CUSTOM_DEPTH_GREEN);
}

UAbilitySystemComponent* ATCFSquadActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UTCFSquadMovementComponent* ATCFSquadActor::GetMovementComponent() const
{
	return MovementComponent;
}

UTCFSquadAttackCommandComponent* ATCFSquadActor::GetAttackCommandComponent() const
{
	return AttackCommandComponent;
}

UTCFSquadIntegrityStateComponent* ATCFSquadActor::GetIntegrityStateComponent() const
{
	return IntegrityStateComponent;
}

UTCFSquadSelectionComponent* ATCFSquadActor::GetSquadSelectionComponent() const
{
	return SquadSelectionComponent;
}

UTCFAffiliationComponent* ATCFSquadActor::GetAffiliationComponent() const
{
	return AffiliationComponent;
}

const UTCFSquadDefinition* ATCFSquadActor::GetSquadDefinition() const
{
	return SquadDefinition;
}

const UTCFSquadAttributeSet* ATCFSquadActor::GetSquadAttributeSet() const
{
	return SquadAttributeSet;
}

FGameplayTag ATCFSquadActor::GetRoleTag() const
{
	return SquadDefinition ? SquadDefinition->RoleTag : FGameplayTag();
}

FText ATCFSquadActor::GetDisplayName() const
{
	return SquadDefinition ? SquadDefinition->DisplayName : FText::GetEmpty();
}

float ATCFSquadActor::GetSelectionRadius() const
{
	return SquadDefinition ? SquadDefinition->SelectionRadius : 0.0f;
}

bool ATCFSquadActor::IsInitialized() const
{
	return bInitialized;
}

void ATCFSquadActor::BeginPlay()
{
	Super::BeginPlay();

	InitializeFromDefinition();

	if (bInitialized)
	{
		if (const UWorld* World = GetWorld())
		{
			if (UTCFSquadQuerySubsystem* SquadQuerySubsystem = World->GetSubsystem<UTCFSquadQuerySubsystem>())
			{
				SquadQuerySubsystem->RegisterSquad(this);
			}
		}
	}
}

void ATCFSquadActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		if (UTCFSquadQuerySubsystem* SquadQuerySubsystem = World->GetSubsystem<UTCFSquadQuerySubsystem>())
		{
			SquadQuerySubsystem->UnregisterSquad(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ATCFSquadActor::InitializeFromDefinition()
{
	if (bInitialized)
	{
		return;
	}

	if (!CanInitializeFromDefinition())
	{
		UE_LOG(LogTemp, Warning, TEXT("TCFSquadActor '%s' failed to initialize. Missing or invalid SquadDefinition."), *GetName());
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributesFromDefinition();
	GrantStartupTags();
	GrantStartupEffects();
	GrantStartupAbilities();

	bInitialized = true;
}

void ATCFSquadActor::InitializeAttributesFromDefinition() const
{
	check(SquadDefinition);
	check(SquadAttributeSet);

	const FTCFSquadAttributeDefaults& Defaults = SquadDefinition->AttributeDefaults;

	SquadAttributeSet->InitMaxHealth(Defaults.MaxHealth);
	SquadAttributeSet->InitHealth(FMath::Clamp(Defaults.Health, 0.0f,	Defaults.MaxHealth));
	SquadAttributeSet->SetMorale(Defaults.Morale);
	SquadAttributeSet->SetSuppression(Defaults.Suppression);
	SquadAttributeSet->SetCohesion(Defaults.Cohesion);
	SquadAttributeSet->SetStamina(Defaults.Stamina);
	SquadAttributeSet->SetAccuracy(Defaults.Accuracy);
	SquadAttributeSet->SetDefense(Defaults.Defense);
	SquadAttributeSet->SetMovementSpeed(Defaults.MovementSpeed);
	SquadAttributeSet->SetCapturePower(Defaults.CapturePower);
}

void ATCFSquadActor::GrantStartupTags() const
{
	check(SquadDefinition);
	check(AbilitySystemComponent);

	if (SquadDefinition->RoleTag.IsValid())
	{
		AbilitySystemComponent->AddLooseGameplayTag(SquadDefinition->RoleTag);
	}

	for (const FGameplayTag& StartupTag : SquadDefinition->StartupTags)
	{
		if (StartupTag.IsValid())
		{
			AbilitySystemComponent->AddLooseGameplayTag(StartupTag);
		}
	}
}

void ATCFSquadActor::GrantStartupEffects() const
{
	if (!HasAuthority())
	{
		return;
	}

	check(SquadDefinition);
	check(AbilitySystemComponent);

	for (const TSubclassOf<UGameplayEffect>& EffectClass : SquadDefinition->StartupEffects)
	{
		if (!EffectClass)
		{
			continue;
		}

		const FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void ATCFSquadActor::GrantStartupAbilities() const
{
	if (!HasAuthority())
	{
		return;
	}

	check(SquadDefinition);
	check(AbilitySystemComponent);

	for (const FTCFSquadAbilityGrant& AbilityGrant : SquadDefinition->StartupAbilities)
	{
		if (!AbilityGrant.AbilityClass)
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec(AbilityGrant.AbilityClass, AbilityGrant.AbilityLevel);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityGrant.InputTag);

		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}

bool ATCFSquadActor::CanInitializeFromDefinition() const
{
	return IsValid(AbilitySystemComponent)
		&& IsValid(SquadAttributeSet)
		&& IsValid(SquadDefinition)
		&& SquadDefinition->IsValidDefinition();
}
