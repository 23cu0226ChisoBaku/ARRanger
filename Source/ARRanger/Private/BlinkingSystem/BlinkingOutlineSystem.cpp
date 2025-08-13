//*************************************************
// アウトラインの点滅処理を制御するシステム
//*************************************************

#include "Public/BlinkingSystem/BlinkingOutlineSystem.h"
#include "Public/BlinkingSystem/BlinkOutlineTickActor.h"
#include "InstantScripts/LineTraceSingleARObjectComponent.h"

FBlinkingOutlineSystem::FBlinkingOutlineSystem()
	: m_TickActor{nullptr}
{
	
}

/*
* @brief アウトラインの点滅処理を行うアクターの生成
*/
void FBlinkingOutlineSystem::CreateTickingActor(UWorld* world, TSubclassOf<ABlinkOutlineTickActor> Subclass)
{	
	if (!m_TickActor.IsValid() && world)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (Subclass == nullptr)
		{
			Subclass = ABlinkOutlineTickActor::StaticClass();
		}
		m_TickActor = world->SpawnActor<ABlinkOutlineTickActor>
			(Subclass, FVector(0.f), FRotator::ZeroRotator, Params);
	}
}

/*
* @brief 対象のオブジェクトを設定するデリゲート関数
*
* @param アウトラインをつける対象のオブジェクトポインタ
*/
void FBlinkingOutlineSystem::SetTargetMagnetizableObjectDelegate(AActor* targetMagnetizableObject)
{
	if(m_TickActor.IsValid())
	{
		if(m_TickActor != nullptr)
		{
			m_TickActor->AddBlinkingActor(targetMagnetizableObject);	
		}
	}
}

/*
* @brief アウトラインの処理を終了させるデリゲート関数
*
* @param アウトラインをはずす対象のオブジェクトポインタ
*/
void FBlinkingOutlineSystem::UnsetTargetMagnetizableObjectDelegate(AActor* targetMagnetizableObject)
{
	if(m_TickActor.IsValid())
	{
		if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(targetMagnetizableObject))
		{
			// 妥協処理
			if(MagnetActor->GetMagnetismType() == EARMagnetismType::None)
			{
				m_TickActor->RemoveBlinkingActor(targetMagnetizableObject);
			}
		}		
	}
}