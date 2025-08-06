//*************************************************
// アウトラインの点滅処理を毎フレーム処理するアクター
//*************************************************

#pragma once

#include "GameFramework/Actor.h"
#include "BlinkOutlineTickActor.generated.h"

UCLASS()
class ARRANGER_API ABlinkOutlineTickActor : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ABlinkOutlineTickActor();
	virtual void Tick(float DeltaTime) override;

	/*
	* @brief 点滅させるアクターを追加する(メッシュコンポーネントを取得する)
	*
	* @param 点滅させるアクター
	*/
	void AddBlinkingActor(AActor* newActor);

	/*
	* @brief 点滅をやめるアクターを配列から除外する(メッシュコンポーネントを除外する)
	*
	* @param 点滅をやめるアクター
	*/
	void RemoveBlinkingActor(AActor* removeActor);

	/*
	* @brief BlinkingOutlineSystem からのリクエストを受け取る
	*/
	void HandleRequest();

private:

	UPROPERTY()
	TArray<TObjectPtr<AActor>> m_BlinkingActors;					// 点滅させるオブジェクトの配列
	UPROPERTY()
	TArray<TObjectPtr<UMeshComponent>> m_BlinkingActorComponents;	// 点滅させるオブジェクトのメッシュコンポーネントの配列
};
