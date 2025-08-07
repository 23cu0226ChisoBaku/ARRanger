//*************************************************
// アウトラインの点滅処理を制御するシステム
//*************************************************

#include "Public/BlinkingSystem/BlinkingOutlineSystem.h"
#include "Public/BlinkingSystem/BlinkOutlineTickActor.h"
#include "InstantScripts/LineTraceSingleARObjectComponent.h"

/*
* @brief アウトラインの点滅処理を行うアクターの生成
*/
void FBlinkingOutlineSystem::CreateTickingActor(UWorld* world)
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
* @brief 対象のオブジェクトを設定するデリゲート関数
*
* @param アウトラインをつける対象のオブジェクトポインタ
*/
void FBlinkingOutlineSystem::SetTargetMagnetizableObjectDelegate(AActor* targetMagnetizableObject)
{
	if(m_TickActor)
	{
		m_TickActor->AddBlinkingActor(targetMagnetizableObject);
	}
}

/*
* @brief アウトラインの処理を終了させるデリゲート関数
*
* @param アウトラインをはずす対象のオブジェクトポインタ
*/
void FBlinkingOutlineSystem::UnsetTargetMagnetizableObjectDelegate(AActor* targetMagnetizableObject)
{
	if(m_TickActor)
	{
		m_TickActor->RemoveBlinkingActor(targetMagnetizableObject);
	}
}