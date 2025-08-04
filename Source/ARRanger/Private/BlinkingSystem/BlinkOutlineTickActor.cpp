//*************************************************
// アウトラインの点滅処理を毎フレーム処理するアクター
//*************************************************

#include "Public/BlinkingSystem/BlinkOutlineTickActor.h"

/*
* @brief 点滅させるアクターを追加する(メッシュコンポーネントを取得する)
*
* @param 点滅させるアクター
*/
void ABlinkOutlineTickActor::AddBlinkingActor(const AActor* newActor)
{
	if (!newActor){ return; }

	UMeshComponent* _pMeshComponent = newActor->FindComponentByClass<UMeshComponent>();

	if (_pMeshComponent || !_pBlinkingActorComponents.Contains(_pMeshComponent)) 
	{
		_pBlinkingActorComponents.Add(_pMeshComponent);
	}
}

/*
* @brief 点滅をやめるアクターを配列から除外する(メッシュコンポーネントを除外する)
*
* @param 点滅をやめるアクター
*/
void ABlinkOutlineTickActor::RemoveBlinkingActor(const AActor* removeActor)
{
	if (!removeActor){ return; }

	UMeshComponent* _pMeshComponent = removeActor->FindComponentByClass<UMeshComponent>();

	if (_pMeshComponent) 
	{
		_pBlinkingActorComponents.Remove(_pMeshComponent);
	}
}


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
}
/*
* End ULineTraceSingleARObjectComponent Lifecycle Functions
*/
