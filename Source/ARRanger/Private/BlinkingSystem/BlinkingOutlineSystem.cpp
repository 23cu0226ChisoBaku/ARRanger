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
	if (!m_TickActor && world)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		m_TickActor = world->SpawnActor<ABlinkOutlineTickActor>
			(ABlinkOutlineTickActor::StaticClass(), FVector(0.f), FRotator::ZeroRotator, Params);
	}
}

/*
* @brief 対象のアクターを設定するデリゲート関数
*
* @param アウトラインをつける対象のオブジェクトポインタ
*/
void BlinkingOutlineSystem::SetTargetMagnetizableObjectDelegate(AActor* magnetizableObject)
{
	AddBlinkingOutlineObject(magnetizableObject);
}

/*
* @brief BlinkOutlineTickActor に対象のアクターを渡す関数
*/
void BlinkingOutlineSystem::AddBlinkingOutlineObject(AActor* magnetizableObject)
{
	if(m_TickActor)
	{
		m_TickActor->AddBlinkingActor(magnetizableObject);
	}
}