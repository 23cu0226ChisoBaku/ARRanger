//*************************************************
// アウトラインの点滅処理を毎フレーム処理するアクター
//*************************************************

#include "Public/BlinkingSystem/BlinkOutlineTickActor.h"

/*
* Start ULineTraceSingleARObjectComponent Lifecycle Functions
*/
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

	//FunFunFunctor()
}
/*
* End ULineTraceSingleARObjectComponent Lifecycle Functions
*/


/*
* @brief 点滅させるアクターを追加する
*
* @param 点滅させるアクター
*/
void ABlinkOutlineTickActor::AddBlinkingActor(AActor* newActor)
{
	if (!newActor){ return; }

	m_BlinkingActors.Add(newActor);

	// 対象アクターのメッシュコンポーネントを保持
	UMeshComponent* _pMeshComponent = newActor->FindComponentByClass<UMeshComponent>();
	if (_pMeshComponent || !m_BlinkingActorComponents.Contains(_pMeshComponent)) 
	{
		m_BlinkingActorComponents.Add(_pMeshComponent);
	}
}

/*
* @brief 点滅をやめるアクターを配列から除外する
*
* @param 点滅をやめるアクター
*/
void ABlinkOutlineTickActor::RemoveBlinkingActor(AActor* removeActor)
{
	if (!removeActor){ return; }

	m_BlinkingActors.Remove(removeActor);

	// 対象アクターのメッシュコンポーネントを除外
	UMeshComponent* _pMeshComponent = removeActor->FindComponentByClass<UMeshComponent>();
	if (_pMeshComponent) 
	{
		m_BlinkingActorComponents.Remove(_pMeshComponent);
	}
}