//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Actors/TCFSquadActor.h"

#include "AbilitySystemComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/TCFSquadDefinition.h"
#include "GAS/TCFSquadAttributeSet.h"
#include "GameplayEffect.h"
#include "Components/TCFSquadSelectionComponent.h"
#include "GASTacticalCommand/GASTacticalCommand.h"

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

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	SquadAttributeSet = CreateDefaultSubobject<UTCFSquadAttributeSet>(TEXT("SquadAttributeSet"));
	
	SquadSelectionComponent = CreateDefaultSubobject<UTCFSquadSelectionComponent>(TEXT("SquadSelectionComponent"));
	
	SquadVisual->SetCustomDepthStencilValue(CUSTOM_DEPTH_GREEN);
}

UAbilitySystemComponent* ATCFSquadActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UTCFSquadSelectionComponent* ATCFSquadActor::GetSquadSelectionComponent() const
{
	return SquadSelectionComponent;
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