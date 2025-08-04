//*************************************************
// アウトラインの点滅処理を制御するシステム
//*************************************************

#include "Public/BlinkingSystem/BlinkingOutlineSystem.h"
#include "Public/BlinkingSystem/BlinkOutlineTickActor.h"
#include "InstantScripts/LineTraceSingleARObjectComponent.h"

/*
* @brief アウトラインの点滅処理を行うアクターの生成
*/
void BlinkingOutlineSystem::CreateTickingActor(UWorld* world)
{	
	if (!_pTickActor && world)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		_pTickActor = world->SpawnActor<ABlinkOutlineTickActor>
			(ABlinkOutlineTickActor::StaticClass(), FVector(0.f), FRotator::ZeroRotator, Params);
	}
}

/*
* @brief 対象のアクターを設定するデリゲート関数
*
* @param アウトラインをつける対象のオブジェクトポインタ
*/
void BlinkingOutlineSystem::SetTargetMagnetizableObject(const AActor* _pMagnetizableObject)
{
	AddBlinkingOutlineObject(_pMagnetizableObject);
}

/*
* @brief BlinkOutlineTickActor に対象のアクターを渡す関数
*/
void BlinkingOutlineSystem::AddBlinkingOutlineObject(const AActor* _pMagnetizableObject)
{
	if(_pTickActor)
	{
		_pTickActor->AddBlinkingActor(_pMagnetizableObject);
	}
}