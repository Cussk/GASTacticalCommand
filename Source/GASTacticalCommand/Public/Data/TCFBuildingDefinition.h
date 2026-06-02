//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Types/TCFEconomyTypes.h"
#include "TCFBuildingDefinition.generated.h"

class ATCFBuildingActor;
class UMaterialInterface;
class UStaticMesh;

UENUM(BlueprintType)
enum class ETCFBuildingRuntimeState : uint8
{
	Inactive,
	UnderConstruction,
	Active,
	Disabled,
	Destroyed
};

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFBuildingDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	FIntPoint GetSafeFootprintSize() const;
	
	UFUNCTION(BlueprintPure, Category = "TCF|Building|Construction")
	float GetSafeRequiredConstructionWork() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Building")
	TSubclassOf<ATCFBuildingActor> GetBuildingActorClass() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building")
	FGameplayTag BuildingTypeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building")
	FGameplayTagContainer BuildingRoleTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building")
	TSubclassOf<ATCFBuildingActor> BuildingActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Economy")
	TArray<FTCFResourceAmount> Cost;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Construction", meta = (ClampMin = "0.0"))
	float RequiredConstructionWork = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Construction")
	bool bCanReceiveWorkerConstruction = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Placement", meta = (ClampMin = "1"))
	FIntPoint FootprintSize = FIntPoint(2, 2);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Placement")
	bool bBlocksBuildingPlacement = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Runtime")
	ETCFBuildingRuntimeState InitialRuntimeState = ETCFBuildingRuntimeState::Active;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Visual")
	TObjectPtr<UStaticMesh> BuildingMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Visual")
	TObjectPtr<UMaterialInterface> BuildingMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Visual")
	FVector BuildingVisualScale = FVector(1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Visual")
	FVector InteractionBoxExtent = FVector(150.0f, 150.0f, 120.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Preview")
	TObjectPtr<UStaticMesh> PreviewMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Preview")
	TObjectPtr<UMaterialInterface> ValidPreviewMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Preview")
	TObjectPtr<UMaterialInterface> InvalidPreviewMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Building|Preview")
	FVector PreviewVisualScale = FVector(1.0f);
};