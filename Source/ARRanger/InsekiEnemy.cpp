#include "InsekiEnemy.h"

#include "Components/BoxComponent.h"

AInsekiEnemy::AInsekiEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentを追加し、RootComponentに設定する
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