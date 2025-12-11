#include "Magnetic/InsekiClimbingObject.h"

#include "Components/BoxComponent.h"

AInsekiClimbingObject::AInsekiClimbingObject()
	: ClimbTrigger(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	ClimbTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ClimbTrigger"));
	RootComponent = ClimbTrigger;

	ClimbTrigger->SetCollisionProfileName(TEXT("Trigger"));
	ClimbTrigger->SetGenerateOverlapEvents(true);
}

void AInsekiClimbingObject::BeginPlay()
{
	Super::BeginPlay();
}

void AInsekiClimbingObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AInsekiClimbingObject::GetClimbNormal()
{
	// �f�t�H���g�ł͏����
	return GetActorUpVector();
}