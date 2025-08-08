//*************************************************
// アウトラインの点滅処理を毎フレーム処理するアクター
//*************************************************

#pragma once

#include "GameFramework/Actor.h"
#include "Public/IARMagnetizableInterface.h"
#include "Public/BlinkingSystem/BlinkDatas.h"
#include "BlinkOutlineTickActor.generated.h"

/**
 * @brief 各磁性に対応する点滅に必要なデータ
 */
USTRUCT(BlueprintType)
struct FBlinkDataSet
{

	GENERATED_BODY()

	// 状態に応じて該当データを返す
	FBlinkingActorData GetBlinkData (EARMagnetismType actorMagType) 
	{
        if(actorMagType == EARMagnetismType::Attraction)
        {            
            return AttractionBlinkData;
        }
        else if(actorMagType == EARMagnetismType::Repulsion)
        {
            return RepulsionBlinkData;
        }
        else
        {
            return NoneBlinkData;
        }
	}

private:

	UPROPERTY(EditAnywhere, Category = "BlinkParam")
	FBlinkingActorData AttractionBlinkData;

	UPROPERTY(EditAnywhere, Category = "BlinkParam")
	FBlinkingActorData RepulsionBlinkData;

	UPROPERTY(EditAnywhere, Category = "BlinkParam")
	FBlinkingActorData NoneBlinkData;
};


/**
 * @brief アウトラインの点滅処理を毎フレーム処理する
 */
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
	UFUNCTION()
	void AddBlinkingActor(AActor* newActor);

	/*
	* @brief 点滅をやめるアクターを配列から除外する(メッシュコンポーネントを除外する)
	*
	* @param 点滅をやめるアクター
	*/
	UFUNCTION()
	void RemoveBlinkingActor(AActor* removeActor);

private:

	/**
	 * @brief 動的なマテリアルを生成する
	 * 
	 * @param 点滅させるマテリアル
	 * 
	 * @return 指定のマテリアルから生成した動的マテリアル
	 */
	UMaterialInstanceDynamic* CreateDynamicMaterial(UMaterialInterface* blinkMaterial);

	UPROPERTY()
	TArray<TObjectPtr<AActor>> m_BlinkingActors;					/*点滅させるオブジェクトの配列*/
	UPROPERTY()
	TArray<TObjectPtr<UMeshComponent>> m_BlinkingActorComponents;	/*点滅させるオブジェクトのメッシュコンポーネントの配列*/ 
	UPROPERTY(EditAnywhere)
	FBlinkDataSet m_BlinkDatas;										/*点滅させる際の必要なパラメータ*/
};
