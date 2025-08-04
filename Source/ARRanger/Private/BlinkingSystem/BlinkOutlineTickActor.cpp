//*************************************************
// アウトラインの点滅処理を毎フレーム処理するアクター
//*************************************************

#include "Public/BlinkingSystem/BlinkOutlineTickActor.h"

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
* @brief 対象オブジェクトをデリゲート関数
*
* @param アウトラインをつける対象のオブジェクトポインタ
*/
void ABlinkOutlineTickActor::SetTargetmagnetizableObject(const AActor* magnetizableObject)
{

}

/*
* @brief BlinkingOutlineSystem からのリクエストを受け取る
*/
void ABlinkOutlineTickActor::HandleRequest()
{
	Add
}

/*
* @brief 点滅させるアクターを追加する(メッシュコンポーネントを取得する)
*
* @param 点滅させるアクター
*/
void ABlinkOutlineTickActor::AddBlinkingActor(AActor* newActor)
{
	if (!newActor){ return; }

	UMeshComponent* MeshComponent = newActor->FindComponentByClass<UMeshComponent>();

	if (MeshComponent || !_pBlinkingActorComponents.Contains(MeshComponent)) 
	{
		_pBlinkingActorComponents.Add(MeshComponent);
	}
}