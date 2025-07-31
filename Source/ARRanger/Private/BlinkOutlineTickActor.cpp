//*************************************************
// アウトラインの点滅処理を毎フレーム呼び出すアクター
//*************************************************

#include "BlinkOutlineTickActor.h"

ABlinkOutlineTickActor::ABlinkOutlineTickActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABlinkOutlineTickActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABlinkOutlineTickActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

