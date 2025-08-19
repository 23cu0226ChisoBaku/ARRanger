//*************************************************
// アウトラインの点滅処理クラス
//*************************************************

#include "Public/BlinkingSystem/BlinkOutlineFunctor.h"
#include "Public/BlinkingSystem/BlinkDatas.h"

// 妥協処理
#include "ARObject/MagnetizableActor.h"

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
void BlinkOutlineFunctor::OutlineBlink(AActor* targetObject, UMeshComponent* targetmeshComponent, FBlinkingActorData* blinkingData, float DeltaTime)
{
    if(targetObject == nullptr || targetmeshComponent == nullptr || blinkingData == nullptr){ return; }

    // 妥協処理
    if(AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(targetObject))
    {
        targetObject = Cast<AMagnetizableActor>(targetObject);
    }

    // 動的マテリアルの生成・セット
    if(targetmeshComponent != nullptr)
    {
        // 妥協処理
        if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(targetObject))
		{
            MagnetActor->DynamicBlinkMaterial = CreateDynamicMaterial(blinkingData->_blinkMaterial, targetObject);
            targetmeshComponent->SetOverlayMaterial(MagnetActor->DynamicBlinkMaterial);
        }
    }

    // 妥協処理(本来はNoneTypeだった場合のみ、なんならここに書きたくない)
    if(blinkingData->_blinkType != EBlinkType::Constant)
    {
        // 妥協処理
        if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(targetObject))
		{
            // 経過時間を加算
            MagnetActor->ElapsedBlinkTime += DeltaTime;

            // ログ表示
            if (GEngine)
            {
                // 第1引数: キー (同じキーなら上書き表示)
                // 第2引数: 表示時間 (秒)
                // 第3引数: 色
                GEngine->AddOnScreenDebugMessage(
                    -1,
                    0.f,
                    FColor::Green,
                    //FString::Printf(TEXT("ElapsedTime: %.2f"), blinkingData->_elapsedTime)
                    FString::Printf(TEXT("ElapsedTime: %.2f"), MagnetActor->ElapsedBlinkTime)
                );
            }
        }
    }

    // ディレイ中なら処理しない
    //if (blinkingData->_elapsedTime < blinkingData->_blinkDelay){ return; }
    // 妥協処理
    if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(targetObject))
    {
        if (MagnetActor->ElapsedBlinkTime < blinkingData->_blinkDelay){ return; }
    }

    // 指定された種類の点滅を行う
    // 等間隔の点滅
    if(blinkingData->_blinkType == EBlinkType::Constant)
    {
        ConstantBlink(targetObject, blinkingData->_blinkInterval, blinkingData->_blinkSpeed);
    }
    // 徐々に早める点滅
    else if(blinkingData->_blinkType == EBlinkType::Accelerated)
    {
        AcceleratedBlink(targetObject, blinkingData->_blinkInterval, blinkingData->_blinkSpeed);
    }
    
    // 点滅時間過ぎたら終了
    if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(targetObject))
    {
        if(blinkingData->_blinkInterval  <=  MagnetActor->ElapsedBlinkTime)
        {
            // コールバック呼び出し
            if (OnBlinkEnd != nullptr)
            {
                if (AActor* targetActor = Cast<AActor>(targetObject))
                {
                    OnBlinkEnd(targetActor);
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
void BlinkOutlineFunctor::ConstantBlink(AActor* targetObject, float blinkInterval, float blinkSpeed)
{
    if (targetObject == nullptr) {return;}

    AMagnetizableActor* magnetActor = Cast<AMagnetizableActor>(targetObject);
    if (magnetActor == nullptr || magnetActor->DynamicBlinkMaterial == nullptr) {return;}

    // 経過時間を加算
    magnetActor->ElapsedBlinkTime += blinkSpeed * targetObject->GetWorld()->GetDeltaSeconds();

    // 点滅計算 (半周期で ON/OFF)
    float phase = FMath::Fmod(magnetActor->ElapsedBlinkTime, blinkInterval);
    float alpha = (phase < blinkInterval / 2.0f) ? 1.0f : 0.0f;

    magnetActor->DynamicBlinkMaterial->SetScalarParameterValue(TEXT("OutlineIntensity"), alpha);
}

/**
 * @brief 指定されたパラメータに応じて徐々に早くなる点滅を行う
 * 
 * @param 
 */
void BlinkOutlineFunctor::AcceleratedBlink(AActor* targetObject, float blinkInterval, float blinkSpeed)
{
    if (targetObject == nullptr) {return;}

    AMagnetizableActor* magnetActor = Cast<AMagnetizableActor>(targetObject);
    if (magnetActor == nullptr || magnetActor->DynamicBlinkMaterial == nullptr) {return;}

    // 経過時間加算
    magnetActor->ElapsedBlinkTime += targetObject->GetWorld()->GetDeltaSeconds();

    // 徐々に早くなる点滅
    float interval = FMath::Max(blinkInterval - magnetActor->ElapsedBlinkTime * blinkSpeed, 0.05f);
    float phase = FMath::Fmod(magnetActor->ElapsedBlinkTime, interval);
    float alpha = (phase < interval / 2.0f) ? 1.0f : 0.0f;

    magnetActor->DynamicBlinkMaterial->SetScalarParameterValue(TEXT("OutlineIntensity"), alpha);
}
