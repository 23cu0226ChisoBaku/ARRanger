//*************************************************
// アウトラインの点滅処理クラス
//*************************************************

#include "Public/BlinkingSystem/BlinkOutlineFunctor.h"
#include "Public/BlinkingSystem/BlinkDatas.h"

/*
* Start BlinkOutlineFunctor Lifecycle Functions
*/
BlinkOutlineFunctor::BlinkOutlineFunctor()
{

}
BlinkOutlineFunctor::~BlinkOutlineFunctor()
{

}
/*
* End BlinkOutlineFunctor Lifecycle Functions
*/


/**
 * @brief 点滅処理を行う関数
 * 
 * @param 点滅処理を行うためのパラメータ,１フレームの時間
 */
void BlinkOutlineFunctor::OutlineBlink(UObject* targetObject, UMeshComponent* targetmeshComponent, FBlinkingActorData* blinkingData, float DeltaTime)
{
    if(targetObject == nullptr || targetmeshComponent == nullptr || blinkingData == nullptr){ return; }

    // 経過時間を加算
    blinkingData->_elapsedTime += DeltaTime;

    // まだディレイ中なら処理しない
    if (blinkingData->_elapsedTime < blinkingData->_blinkDelay){ return; }

    // 動的マテリアルの生成・マテリアルのセット
    if(blinkingData->m_DynamicMaterial == nullptr)
    {
        blinkingData->m_DynamicMaterial = CreateDynamicMaterial(blinkingData->_blinkMaterial, targetObject);
        if (blinkingData->m_DynamicMaterial)
        {
            targetmeshComponent->SetOverlayMaterial(blinkingData->m_DynamicMaterial);
        }
    }

    // 指定された種類の点滅を行う
    // 等間隔の点滅
    if(blinkingData->_blinkType == EBlinkType::Constant)
    {
        ConstantBlink();
    }
    // 徐々に早める点滅
    else if(blinkingData->_blinkType == EBlinkType::Accelerated)
    {
        AcceleratedBlink();
    }

    // 妥協処理(本来はNoneTypeだった場合のみ、なんならここに書きたくない)
    if(blinkingData->_blinkType != EBlinkType::Constant)
    {
        // 点滅する時間が過ぎていたら点滅終了(点滅し始めてからの経過時間で計測)
        if(blinkingData->_blinkInterval  <=  blinkingData->_elapsedTime - blinkingData->_blinkDelay)
        {
            // コールバック呼び出し
            if (OnBlinkEnd)
            {
                if (AActor* targetActor = Cast<AActor>(targetObject))
                {
                    //OnBlinkEnd(targetActor);
                }
            }
        }
    }
} 

/**
 * @brief 動的なマテリアルを生成する
 * 
 * @param 点滅させるマテリアル
 * 
 * @return 指定のマテリアルから生成した動的マテリアル, 
 */
UMaterialInstanceDynamic* BlinkOutlineFunctor::CreateDynamicMaterial(UMaterialInterface* blinkMaterial, UObject* targetObject)
{
    if (blinkMaterial == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateDynamicMaterial(): bnlinkmaterial is null"));
        return nullptr;
    }

    UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(blinkMaterial, targetObject);
    if (DynamicMat == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateDynamicMaterial(): Failed to create dynamic material instance"));
        return nullptr;
    }

    return DynamicMat;
}

/**
 * @brief 指定されたパラメータに応じて等間隔で点滅を行う
 * 
 * @param 
 */
void BlinkOutlineFunctor::ConstantBlink()
{

}

/**
 * @brief 指定されたパラメータに応じて徐々に早くなる点滅を行う
 * 
 * @param 
 */
void BlinkOutlineFunctor::AcceleratedBlink()
{
    
}
