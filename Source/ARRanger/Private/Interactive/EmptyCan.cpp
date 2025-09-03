//*************************************************
// 空き缶インスタンス
//*************************************************

#include "Interactive/EmptyCan.h"

AEmptyCan::AEmptyCan()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
  RootComponent = MeshComponent;
}
void AEmptyCan::BeginPlay()
{
	Super::BeginPlay();
}
void AEmptyCan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEmptyCan::OnPreAttacked(const FARAttackParameters& InAttackParams, ARRanger::Battle::FARAttackResult& OutAttackResult)
{
  // Always make attack success
  OutAttackResult.Result = ARRanger::Battle::EARAttackResult::Success;
}

void AEmptyCan::OnPostAttacked(const FARAttackParameters& InAttackParams)
{
  if ((MeshComponent != nullptr) && MeshComponent->IsSimulatingPhysics())
  {
    const float power = 1500.0f;
    MeshComponent->AddImpulse(InAttackParams.LaunchDirection * power, NAME_None, true);
  }
}

/**
 * @brief ダメージ量に適した飛ぶ力を取得する
 * 
 * @return 飛ぶ力
 */
UFUNCTION()
float AEmptyCan::GetFlyForceByDamage()
{
	return 0.0f;
}

