#include "Enemy.h"

#include "Components/BoxComponent.h"

AEnemy::AEnemy()
	: InsekiActorMesh(nullptr)
	, Box(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponent‚ğ’Ç‰Á‚µARootComponent‚Éİ’è‚·‚é
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// BoxComponent‚ğ’Ç‰Á‚µABoxComponent‚ğRootComponent‚ÉAttach‚·‚é
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	Box->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}