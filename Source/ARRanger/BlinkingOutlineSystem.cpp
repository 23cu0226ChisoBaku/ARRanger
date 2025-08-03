//*************************************************
// アウトラインの点滅処理を制御するシステム
//*************************************************

#include "BlinkingOutlineSystem.h"

/*
* @brief アウトラインの点滅処理を行うアクターの生成
*/
AActor* BlinkingOutlineSystem::CreateTickingActor(const UWorld* world)
{	
	if (!TickActor && world)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		return world->SpawnActor<ABlinkOutlineTickActor>(ABlinkOutlineTickActor::StaticClass(), FVector(0.f), FRotator::ZeroRotator, Params);
	}

	return nullptr;
}

/*
* @brief アウトラインの処理を呼び出すデリゲート関数
*
* @param アウトラインをつける対象のオブジェクトポインタ
*/
void OnBlinkingOutline(AActor* magnetizableObject)
{

}


//void BlinkingOutlineSystem::RegisterOnOutline();

//void BlinkingOutlineSystem::RequestOnBlinkingOutLine();
