//*************************************************
// 引力必殺技の対象オブジェクトを吸引するアクター
//*************************************************

#include "SpecialAttackAttractActor.h"

ASpecialAttackAttractActor::ASpecialAttackAttractActor()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ASpecialAttackAttractActor::BeginPlay()
{
	Super::BeginPlay();
}
void ASpecialAttackAttractActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if true

#else

#endif
}