//*************************************************
// アウトラインの点滅処理を制御するシステム
//*************************************************

#include "Public/BlinkingSystem/BlinkingOutlineSystem.h"

/*
* @brief アウトラインの点滅処理を行うアクターの生成
*/
void BlinkingOutlineSystem::CreateTickingActor(UWorld* world)
{	
	if (!TickActor && world)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		_pTickActor = world->SpawnActor<ABlinkOutlineTickActor>
			(ABlinkOutlineTickActor::StaticClass(), FVector(0.f), FRotator::ZeroRotator, Params);
	}
}

/*
* @brief アウトラインの処理を呼び出すデリゲート関数
*
* @param アウトラインをつける対象のオブジェクトポインタ
*/
void SetTargetMagnetizableObject(AActor* magnetizableObject)
{

}

/*
* @brief BlinkOutlineTickActor に点滅処理をリクエスト
*/
void BlinkingOutlineSystem::void RequestToBlinkingOutlineTickActor(ABlinkOutlineTickActor tickActor)
{
	if(_pTickActor)
	{
		_pTickActor->
	}
}
