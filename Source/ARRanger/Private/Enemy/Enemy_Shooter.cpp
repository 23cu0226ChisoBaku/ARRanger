#include "Enemy/Enemy_Shooter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

AEnemy_Shooter::AEnemy_Shooter()
{
  IdealRange = 1000.f;
  RangeTolerance = 200.f;
  PreferredDistance = 200.f;
  
  MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
  MuzzlePoint->SetupAttachment(GetMesh(), TEXT("MuzzleSocket")); 
}

void AEnemy_Shooter::BeginPlay()
{
  Super::BeginPlay();
}
