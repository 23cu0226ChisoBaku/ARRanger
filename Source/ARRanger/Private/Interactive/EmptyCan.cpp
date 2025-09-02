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

