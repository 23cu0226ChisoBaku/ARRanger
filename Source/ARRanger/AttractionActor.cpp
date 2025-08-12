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

	SetMagnetismType(EARMagnetismType::Attraction);
}


