#include "AttractionActor.h"

#include "ARRangerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AAttractionActor::AAttractionActor()
	: constProp(25.0f)
	, magneticValue(20.0f)
	, onStayFlag(false)
	, repulsionFlag(false)
	, playerCharacter(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentÇí«â¡ÇµÅARootComponentÇ…ê›íËÇ∑ÇÈ
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// BoxComponentÇí«â¡ÇµÅABoxComponentÇRootComponentÇ…AttachÇ∑ÇÈ
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("BoxComponent"));
	Sphere->SetupAttachment(RootComponent);
}

void AAttractionActor::BeginPlay()
{
	Super::BeginPlay();

	// OnComponentBeginOverlapÇBindÇ∑ÇÈ
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAttractionActor::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AAttractionActor::OnSphereEndOverlap);
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