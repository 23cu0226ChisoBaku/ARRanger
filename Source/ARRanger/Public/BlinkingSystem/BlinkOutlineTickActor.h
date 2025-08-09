//*************************************************
// アウトラインの点滅処理を毎フレーム処理するアクター
//*************************************************

#pragma once

#include "GameFramework/Actor.h"
#include "Public/IARMagnetizableInterface.h"
#include "Public/BlinkingSystem/BlinkDatas.h"
#include "ARObject/MagnetizableActor.h"
#include "BlinkOutlineTickActor.generated.h"

// 前方宣言
class BlinkOutlineFunctor;

/**
 * @brief 点滅処理の対象アクターを保持する
 */
USTRUCT()
struct FBlinkingTarget
{
	GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<AActor> _actor = nullptr;

    UPROPERTY()
    TObjectPtr<UMeshComponent> _meshComponent = nullptr;
};

/**
 * @brief 各磁性に対応する点滅に必要なデータ
 */
USTRUCT(BlueprintType)
struct FBlinkDataSet
{
	GENERATED_BODY()

	// 状態に応じて該当データを返す
	FBlinkingActorData* GetBlinkData (AActor* blinkActor) 
	{
		if(blinkActor == nullptr){ return nullptr; }

		EARMagnetismType actorMagType = EARMagnetismType::None;

		if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(blinkActor))
		{
			actorMagType = MagnetActor->GetMagnetismType();
		}

        if(actorMagType == EARMagnetismType::Attraction)
        {            
            return &AttractionBlinkData;
        }
        else if(actorMagType == EARMagnetismType::Repulsion)
        {
            return &RepulsionBlinkData;
        }
        else
        {
            return &NoneBlinkData;
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
	~ABlinkOutlineTickActor();
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
	 * @brief m_BlinkingActors構造体配列の中に指定したアクターが存在するかどうかを返す
	 * 
	 * @param 探したいアクター
	 * 
	 * @return 存在したいかどうか
	 */
	bool ContainsActor(AActor* actor);

	UPROPERTY()
	TArray<FBlinkingTarget> m_BlinkingActors;						/*点滅させる対象のアクターのデータ構造体*/
	UPROPERTY(EditAnywhere)
	FBlinkDataSet m_BlinkDatas;										/*点滅させる際の必要なパラメータ*/

	BlinkOutlineFunctor* m_BlinkOutlineFunctor;						/*点滅する処理があるFunctorクラス*/
};