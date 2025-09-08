//*************************************************
// アウトラインの点滅処理クラス
//*************************************************

#include "Public/BlinkingSystem/BlinkOutlineFunctor.h"
#include "Public/BlinkingSystem/BlinkDatas.h"
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
    // /*デバッグ用*/
	// if (GEngine)
	// {
    //     GEngine->AddOnScreenDebugMessage(
    //         -1,                // Key (-1 = 自動割り当て)
    //         1.0f,              // 表示時間（秒）
    //         FColor::Green,     // 色
    //         TEXT("SSSSS")              // 表示内容
    //     );
	// } 

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

    // 妥協処理
    if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(targetObject))
    {
        // 経過時間を加算
        MagnetActor->ElapsedBlinkTime += DeltaTime;

        // /*デバッグ用*/
        // if (GEngine)
        // {
        //     // 第1引数: キー (同じキーなら上書き表示)
        //     // 第2引数: 表示時間 (秒)
        //     // 第3引数: 色
        //     GEngine->AddOnScreenDebugMessage(
        //         -1,
        //         0.f,
        //         FColor::Green,
        //         //FString::Printf(TEXT("ElapsedTime: %.2f"), blinkingData->_elapsedTime)
        //         FString::Printf(TEXT("ElapsedTime: %.2f"), MagnetActor->ElapsedBlinkTime)
        //     );
        // }
    } 

    // ディレイ中は処理をしない
    if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(targetObject))
    {
        if(MagnetActor->ElapsedBlinkTime <= blinkingData->_blinkDelay)
        {
            return;
        }
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
        // ディレイ時間を含めた点滅時間を過ぎるまで処理しない
        if(blinkingData->_blinkInterval + blinkingData->_blinkDelay  <=  MagnetActor->ElapsedBlinkTime)
        {
            // 点滅処理を止めて登録リストから削除
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
 * @brief 点滅処理を行う関数
 *
 * @param 対象アクター
 */
void BlinkOutlineFunctor::ResetMaterialParam(AActor* targetActor)
{
    if (targetActor == nullptr) {return;}

    AMagnetizableActor* magnetActor = Cast<AMagnetizableActor>(targetActor);
    if (magnetActor == nullptr || magnetActor->DynamicBlinkMaterial == nullptr) {return;}

    magnetActor->ElapsedBlinkTime = 0.0f;
    magnetActor->DynamicBlinkMaterial->SetScalarParameterValue(TEXT("BlinkAlpha"), -1.0f);
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

    // 点滅計算 (半周期で ON/OFF)
    float phase = FMath::Fmod(magnetActor->ElapsedBlinkTime * blinkSpeed, blinkInterval);
    float alpha = (phase < blinkInterval / 2.0f) ? 1.0f : 0.0f;

    // FLinearColor outlineColor{};
    // FHashedMaterialParameterInfo paramInfo{TEXT("OutlineColor")};
    // if (!magnetActor->DynamicBlinkMaterial->GetVectorParameterValue(paramInfo ,outlineColor))
    // {
    //     UE_LOG(LogTemp, Error, TEXT("OSOI!!!!!"));
    //     return;
    // }

    // static float s_TempTime = 0.0f;
    // s_TempTime += FApp::GetDeltaTime();

    // outlineColor *= FMath::Abs(FMath::Cos(s_TempTime * 3.1415926535f)) * 10;

    // // TODO Hard coding
    // magnetActor->DynamicBlinkMaterial->SetVectorParameterValue(TEXT("OutlineColor"), outlineColor);

    // TODO Hard coding
    magnetActor->DynamicBlinkMaterial->SetScalarParameterValue(TEXT("BlinkAlpha"), alpha);

    // /*デバッグ用*/
	// if (GEngine)
	// {
    //     GEngine->AddOnScreenDebugMessage(
    //         -1,                // Key (-1 = 自動割り当て)
    //         5.0f,              // 表示時間（秒）
    //         FColor::Green,     // 色
    //         FString::Printf(TEXT("alpha: %.2f"), alpha)              // 表示内容
    //     );
	// }
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

    // 徐々に早くなる点滅
    float interval = FMath::Max(blinkInterval - magnetActor->ElapsedBlinkTime * blinkSpeed, 0.05f);
    float phase = FMath::Fmod(magnetActor->ElapsedBlinkTime, interval);
    float alpha = (phase < interval / 2.0f) ? 1.0f : 0.0f;

    magnetActor->DynamicBlinkMaterial->SetScalarParameterValue(TEXT("BlinkAlpha"), alpha);

    // /*デバッグ用*/
	// if (GEngine)
	// {
    //     GEngine->AddOnScreenDebugMessage(
    //         -1,                // Key (-1 = 自動割り当て)
    //         5.0f,              // 表示時間（秒）
    //         FColor::Green,     // 色
    //         FString::Printf(TEXT("alpha: %.2f"), alpha)              // 表示内容
    //     );
	// }       
}
