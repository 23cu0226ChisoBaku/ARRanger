#include "InsekiEnemy.h"

#include "Components/BoxComponent.h"

AInsekiEnemy::AInsekiEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponent��ǉ����ARootComponent�ɐݒ肷��
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;
}

void AInsekiEnemy::BeginPlay()
{
	Super::BeginPlay();
	

}

void AInsekiEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInsekiEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}