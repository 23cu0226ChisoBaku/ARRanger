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

    UPROPERTY()
    float _elapsedTime = 0.0f;                              /*経過時間*/
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> m_DynamicMaterial;	/*動的マテリアル*/
    UPROPERTY(EditAnywhere)
    float _blinkDelay;                                      /*点滅するまでの遅延*/
    UPROPERTY(EditAnywhere)
    float _blinkSpeed;                                      /*点滅スピード*/
    UPROPERTY(EditAnywhere)
    EBlinkType _blinkType;                                  /*点滅の仕方*/
    UPROPERTY(EditAnywhere)
    TObjectPtr<UMeshComponent> _meshComponent;              /*マテリアルを適用するメッシュコンポ―ネント*/
    UPROPERTY(EditAnywhere)
    TObjectPtr<UMaterialInterface> _blinkMaterial;          /*点滅させるマテリアル*/
};