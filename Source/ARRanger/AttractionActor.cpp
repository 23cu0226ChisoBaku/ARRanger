#include "AttractionActor.h"

#include "ARRangerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AAttractionActor::AAttractionActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponent��ǉ����ARootComponent�ɐݒ肷��
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// BoxComponent��ǉ����ABoxComponent��RootComponent��Attach����
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("BoxComponent"));
	Sphere->SetupAttachment(RootComponent);
}

void AAttractionActor::BeginPlay()
{
	Super::BeginPlay();

	// OnComponentBeginOverlap��Bind����
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAttractionActor::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AAttractionActor::OnSphereEndOverlap);

  SetMagnetismType(EARMagnetismType::Attraction);
}

void AAttractionActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  IARMagnetizableInterface* otherMagnetized = Cast<IARMagnetizableInterface>(OtherActor);
  if (otherMagnetized != nullptr)
  {
    Physics_RequestMagneticTask(this, otherMagnetized);
  } 
}

void AAttractionActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
  IARMagnetizableInterface* otherMagnetized = Cast<IARMagnetizableInterface>(OtherActor);
  if (otherMagnetized != nullptr)
  {
    Physics_TerminateMagneticTask(this, otherMagnetized);
  } 
}