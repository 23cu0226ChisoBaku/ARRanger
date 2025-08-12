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
 * @brief 点滅する色
 */
UENUM(BlueprintType)
enum class EBlinkColorType : uint8
{
	White  UMETA(DisplayName = "White"),
    Blue   UMETA(DisplayName = "Blue"),
    Red    UMETA(DisplayName = "Red"),
};

/**
 * @ brief 点滅処理に必要なパラメータ
 */
USTRUCT(BlueprintType)
struct FBlinkingActorData
{
	GENERATED_BODY()
	
    UPROPERTY()
    TWeakObjectPtr<UMeshComponent> _targetMeshComp;
    UPROPERTY()
    float _elapsedTime = 0.0f;
    UPROPERTY()
    float _blinkDelay = 0.0f;
    UPROPERTY()
    float _blinkSpeed = 1.0f;
    UPROPERTY()
    EBlinkType _blinkType = EBlinkType::Constant;
    UPROPERTY()
    EBlinkColorType _blinkColor = EBlinkColorType::White;
};
