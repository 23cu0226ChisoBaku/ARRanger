#include "InsekiEnemy.h"

#include "Components/BoxComponent.h"

AInsekiEnemy::AInsekiEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentÇí«â¡ÇµÅARootComponentÇ…ê›íËÇ∑ÇÈ
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