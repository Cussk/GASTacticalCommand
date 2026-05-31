//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "TCFResourceNodeDefinition.generated.h"

class UMaterialInterface;
class UStaticMesh;

UENUM(BlueprintType)
enum class ETCFResourceNodeDepletionBehavior : uint8
{
	StayEmpty,
	Hide,
	Destroy
};

UCLASS(BlueprintType)
class GASTACTICALCOMMAND_API UTCFResourceNodeDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TCF|Resource Node")
	int32 RollInitialAmount() const;

	UFUNCTION(BlueprintPure, Category = "TCF|Resource Node")
	int32 GetSafeGatherAmount() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node")
	FGameplayTag ResourceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node", meta = (ClampMin = "0"))
	int32 MinimumInitialAmount = 500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node", meta = (ClampMin = "0"))
	int32 MaximumInitialAmount = 500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node", meta = (ClampMin = "1"))
	int32 GatherAmountPerAction = 25;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node|Visual")
	TObjectPtr<UStaticMesh> VisualMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node|Visual")
	TObjectPtr<UMaterialInterface> VisualMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node|Visual")
	FVector VisualScale = FVector(1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node|Depleted")
	ETCFResourceNodeDepletionBehavior DepletionBehavior = ETCFResourceNodeDepletionBehavior::StayEmpty;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TCF|Resource Node|Depleted", meta = (ClampMin = "0.0"))
	float DestroyDelaySeconds = 0.0f;
};