//*************************************************
// アウトラインの点滅処理を毎フレーム処理するアクター
//*************************************************

#include "Public/BlinkingSystem/BlinkOutlineTickActor.h"

#define OUTLINEMATERIAL

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

	//OutlineBlink(m_BlinkDatas)
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
	if (!newActor || m_BlinkingActors.Contains(newActor)){ return; }

	// アクターを対象オブジェクトに追加
	m_BlinkingActors.Add(newActor);

	// 対象アクターのメッシュコンポーネントを追加
	UMeshComponent* meshComponent = newActor->FindComponentByClass<UMeshComponent>();

	if (meshComponent || !m_BlinkingActorComponents.Contains(meshComponent)) 
	{
		if(meshComponent->ComponentHasTag("OutLineMesh"))
		{
			m_BlinkingActorComponents.Add(meshComponent);
		}		
	}
}

/*
* @brief 点滅をやめるアクターを配列から除外する
*
* @param 点滅をやめるアクター
*/
void ABlinkOutlineTickActor::RemoveBlinkingActor(AActor* removeActor)
{
	if (!removeActor || !m_BlinkingActors.Contains(removeActor)){ return; }

	// アクターを対象オブジェクトから除外
	m_BlinkingActors.Remove(removeActor);

	// 対象アクターのメッシュコンポーネントを除外
	UMeshComponent* meshComp = removeActor->FindComponentByClass<UMeshComponent>();
	if (meshComp)
	{	
		if(meshComp->ComponentHasTag("OutLineMesh"))
		{
			m_BlinkingActorComponents.Remove(meshComp);
		}		
	}
}