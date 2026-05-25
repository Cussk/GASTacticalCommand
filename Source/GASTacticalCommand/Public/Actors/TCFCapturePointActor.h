//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TCFCapturePointActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
class UTCFAffiliationComponent;
class UTCFCapturePointComponent;

UCLASS()
class GASTACTICALCOMMAND_API ATCFCapturePointActor : public AActor
{
	GENERATED_BODY()

public:
	ATCFCapturePointActor();

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = "TCF|Components")
	UTCFAffiliationComponent* GetAffiliationComponent() const;

	UFUNCTION(BlueprintCallable, Category = "TCF|Components")
	UTCFCapturePointComponent* GetCapturePointComponent() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UStaticMeshComponent> PointVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<USphereComponent> CaptureRadiusPreview;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFAffiliationComponent> AffiliationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TCF|Components")
	TObjectPtr<UTCFCapturePointComponent> CapturePointComponent;
};