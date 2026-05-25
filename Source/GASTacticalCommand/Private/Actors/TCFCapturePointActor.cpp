//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Actors/TCFCapturePointActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TCFAffiliationComponent.h"
#include "Components/TCFCapturePointComponent.h"

ATCFCapturePointActor::ATCFCapturePointActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PointVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PointVisual"));
	PointVisual->SetupAttachment(SceneRoot);
	PointVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CaptureRadiusPreview = CreateDefaultSubobject<USphereComponent>(TEXT("CaptureRadiusPreview"));
	CaptureRadiusPreview->SetupAttachment(SceneRoot);
	CaptureRadiusPreview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CaptureRadiusPreview->SetSphereRadius(500.0f);

	AffiliationComponent = CreateDefaultSubobject<UTCFAffiliationComponent>(TEXT("AffiliationComponent"));
	CapturePointComponent = CreateDefaultSubobject<UTCFCapturePointComponent>(TEXT("CapturePointComponent"));
}

void ATCFCapturePointActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (CaptureRadiusPreview && CapturePointComponent)
	{
		CaptureRadiusPreview->SetSphereRadius(CapturePointComponent->GetCaptureRadius());
	}
}

UTCFAffiliationComponent* ATCFCapturePointActor::GetAffiliationComponent() const
{
	return AffiliationComponent;
}

UTCFCapturePointComponent* ATCFCapturePointActor::GetCapturePointComponent() const
{
	return CapturePointComponent;
}
