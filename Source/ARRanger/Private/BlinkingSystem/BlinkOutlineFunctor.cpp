//*************************************************
// アウトラインの点滅処理クラス
//*************************************************

#include "Public/BlinkingSystem/BlinkOutlineFunctor.h"
#include "Public/BlinkingSystem/BlinkDatas.h"

BlinkOutlineFunctor::BlinkOutlineFunctor()
{
}

BlinkOutlineFunctor::~BlinkOutlineFunctor()
{
}

/**
 * @brief 点滅処理を行う関数
 * 
 * @param 点滅処理を行うためのパラメータ,１フレームの時間
 */
void BlinkOutlineFunctor::OutlineBlink(FBlinkingActorData* blinkingData, float DeltaTime, UObject* targetObject)
{
    if(blinkingData == nullptr){ return; }

    // 経過時間を加算
    blinkingData->_elapsedTime += DeltaTime;

    // まだディレイ中なら処理しない
    if (blinkingData->_elapsedTime < blinkingData->_blinkDelay){ return; }
} 

/**
 * @brief 動的なマテリアルを生成する
 * 
 * @param 点滅させるマテリアル
 * 
 * @return 指定のマテリアルから生成した動的マテリアル, 
 */
UMaterialInstanceDynamic* CreateDynamicMaterial(UMaterialInterface* blinkMaterial, UObject* targetObject)
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