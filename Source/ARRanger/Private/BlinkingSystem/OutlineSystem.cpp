//*************************************************
// アウトラインの表示処理を制御するシステム
//*************************************************

#include "Public/BlinkingSystem/OutlineSystem.h"
#include "Public/BlinkingSystem/OutlineTickActor.h"
#include "ARObject/MagnetizableActor.h"

FOutlineSystem::FOutlineSystem()
	: m_TickActor{ nullptr }
{
}

/*
* @brief アウトラインの点滅処理を行うアクターの生成
*/
void FOutlineSystem::CreateTickingActor(UWorld* world, TSubclassOf<AOutlineTickActor> Subclass)
{
	if (!m_TickActor.IsValid() && world)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (Subclass == nullptr)
		{
			Subclass = AOutlineTickActor::StaticClass();
		}
		m_TickActor = world->SpawnActor<AOutlineTickActor>(
			Subclass, FVector(0.f), FRotator::ZeroRotator, Params
		);

		// アウトラインを適用させるオブジェクト配列のポインタを渡す
		if (m_TickActor.IsValid())
		{
			m_TickActor->BindOutlineActorArrays(&m_OutlineActors, &m_BlinkingOutlineActors);
			// コールバックを登録
			m_TickActor->OnRequestRemoveBlinkingActor = [this](AActor* actor)
			{
				this->RemoveBlinkingOutlineActor(actor);
			};
		}
	}
}

/*
* @brief カーソルのあっているオブジェクトを白く点滅させるデリゲート関数
*
* @param 点滅するアウトラインをつける対象のオブジェクトポインタ
*/
void FOutlineSystem::BlinkOutlineActorAtCursorDelegate(AActor* targetObject)
{
	if(targetObject != nullptr)
	{
		m_TickActor->SetBlinkOutlineActorAtCursor(targetObject);			
	}
}

/*
* @brief カーソルが外れたオブジェクトの点滅を終了させるデリゲート関数
*
* @param 点滅するアウトラインをつける対象のオブジェクトポインタ
*/
void FOutlineSystem::StopBlinkOutlineActorAtCursorDelegate(AActor* targetObject)
{
	if(targetObject != nullptr)
	{
		m_TickActor->UnsetBlinkOutlineActorAtCursor(targetObject);			
	}
	else
	{
		m_TickActor->UnsetBlinkOutlineActorAtCursor(nullptr);
	}
}

/*
* @brief アウトラインを付ける対象オブジェクトを設定するデリゲート関数
*
* @param アウトラインをつける対象のオブジェクトポインタ
*/
void FOutlineSystem::AddOutlineActorDelegate(AActor* targetObject)
{
	if (targetObject != nullptr && !m_OutlineActors.Contains(targetObject))
	{
		m_OutlineActors.Add(targetObject);
		m_TickActor->OnIsUpOutlineActors();
	}
}

/*
* @brief アウトラインを外す対象オブジェクトを削除するデリゲート関数
*
* @param アウトラインを外す対象のオブジェクトポインタ
*/
void FOutlineSystem::RemoveOutlineActorDelegate(AActor* targetObject)
{
    if(targetObject != nullptr && m_OutlineActors.Contains(targetObject))
    {
        m_OutlineActors.Remove(targetObject);
		m_TickActor->OnIsUpOutlineActors();
    }
}

/*
* @brief 点滅アウトライン対象を追加するデリゲート関数
*
* @param 点滅アウトラインをつける対象のオブジェクトポインタ 
*/
void FOutlineSystem::AddBlinkingOutlineActorDelegate(AActor* targetObject)
{
	if (targetObject == nullptr || !m_TickActor.IsValid() || m_TickActor == nullptr){return;}

	// すでに点滅アウトラインに入っていなければ追加
	if (!m_BlinkingOutlineActors.Contains(targetObject))
	{
		m_BlinkingOutlineActors.Add(targetObject);
		m_TickActor->AddBlinkingActor(targetObject);
		m_TickActor->OnIsUpBlinkingActors();
	}
}

/*
* @brief 点滅アウトライン対象を削除する
*
* @param 点滅アウトラインをはずす対象のオブジェクトポインタ
*/
void FOutlineSystem::RemoveBlinkingOutlineActor(AActor* targetObject)
{
	if (m_TickActor.IsValid())
	{
		if (m_TickActor != nullptr)
		{
			m_BlinkingOutlineActors.Remove(targetObject);
		}
	}
}