

#include "BlinkOutlineTickActor.h"
//*************************************************
// アウトラインの点滅処理を毎フレーム処理するアクター
//*************************************************

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
* @brief アウトラインの処理を呼び出すデリゲート関数
*
* @param アウトラインをつける対象のオブジェクトポインタ
*/
void ABlinkOutlineTickActor::OnBlinkingOutline(AActor* magnetizableObject)
{

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



