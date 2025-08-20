//*************************************************
// 点滅処理で必要なパラメータをまとめたクラス
//*************************************************

#pragma once

#include "BlinkDatas.generated.h"

/**
 * @brief 点滅の仕方
 */
UENUM(BlueprintType)
enum class EBlinkType : uint8
{
    Constant     UMETA(DisplayName = "Constant Blink"),
    Accelerated  UMETA(DisplayName = "Accelerated Blink"),
};

/**
 * @brief 点滅処理に必要なパラメータ
 */
USTRUCT(BlueprintType)
struct FBlinkingActorData
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    float _blinkInterval;                                               /*点滅する時間*/ 
    UPROPERTY(EditAnywhere)
    float _blinkSpeed;                                                  /*点滅スピード*/
    UPROPERTY(EditAnywhere)
    float _blinkDelay;                                                  /*点滅し始めるまでの遅延*/
    UPROPERTY(EditAnywhere)
    EBlinkType _blinkType;                                              /*点滅の仕方*/
    UPROPERTY(EditAnywhere)
    TObjectPtr<UMaterialInterface> _blinkMaterial;                      /*点滅させるマテリアル*/
};