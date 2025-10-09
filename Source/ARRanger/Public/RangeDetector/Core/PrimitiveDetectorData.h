/**
 * @file PrimitiveDetectorData.h
 * @brief Base class of all detector data
 */

#pragma once

#include "Engine/DataAsset.h"

#include "RangeDetector/Core/DetectorImpls.h"
#include "GameplayTagContainer.h"

#include <type_traits>

#include "PrimitiveDetectorData.generated.h"

/**Forward declaration */
class UWorld;

#define UE_API ARRANGER_API

/**
 * @brief Definition of interface.Use this inside derived class (.h file)
 */
/**
 * @brief 範囲探知インターフェイスの宣言。子クラスで使用
 */
#define DECLARE_PRIMITIVE_DETECTOR(DetectorType) \
  virtual int32 DetectTargets(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, TArray<AActor*>& OutResult) const override;\

/**
 * @brief Declarations of DECLARE_PRIMITIVE_DETECTOR.Use this for derived class (.cpp file)
 */
/**
 * @brief 範囲探知インターフェイスの定義。cppファイル内で使用
 */
#define DEFINE_PRIMITIVE_DETECTOR(DetectorType) \
  int32 DetectorType::DetectTargets(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, TArray<AActor*>& OutResult) const \
  { \
    static_assert(std::is_base_of_v<UPrimitiveDetectorData, DetectorType>, "Invalid type, Use DERIVED class of UPrimitiveDetectorData"); \
    check(World != nullptr); \
    return ARRanger::Detector::DetectTargetsImpl( World, OriginActor, InOriginLocation, InOriginRotation, InOriginScale3D, *this, OutResult); \
  }
  
/**
 * @brief Base class of all detector data
 * Inherit this for specific type(Sphere,Capsule,etc...) use.
 */
/**
 * @brief 範囲を探知する機能のデータベースクラス。
 * 全ての具体化データクラス（スフィアやカプセルなど）はこのクラスを継承
 */
UCLASS(Abstract, Const)
class UPrimitiveDetectorData : public UDataAsset
{
	GENERATED_BODY()
	
public:

  /**Tag of this data */
  /**データタグ */
  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|DetectorData")
  FGameplayTag DataTag;

  /**Position offset of center point */
  /**中央座標からのオフセット */
  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|DetectorData")
  FVector CenterPositionOffset;

  /**Rotation offset */
  /**回転のオフセット */
  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|DetectorData")
  FRotator RotationOffset;

/**Editor debug purpose */
/**エディタデバッグ用 */
#if WITH_EDITORONLY_DATA

  /**
   * @see function: ::DebugDrawRange()
   */
  /**デバッグラインの色 */
  UPROPERTY(EditDefaultsOnly, Category = "Debug")
  FLinearColor DebugLineColor = FLinearColor::Red;
#endif

  /**
   * @brief Detect targets inside range.Always ignore OriginActor(Self)
   * @param World            World
   * @param OriginActor      User actor pointer
   * @param InOriginLocation Detector shape center location(World position).Use as start position.
   * @param InOriginRotation Detector shape rotation(World rotation).
   * @param InOriginScale3D  Detector shape scale(World scale).
   * @param OutResult        Detected actors will store in this array.
   * 
   * @return                 Num of detected actors.Return 0 if nothing detected. 
   */
  /**
   * @brief 探知範囲内のターゲットを取得する関数。常にOriginActor(自分)を無視する
   * @param World ワールド
   * @param OriginActor      利用者アクターポインター
   * @param InOriginLocation 探知範囲の形状の中心座標（ワールド）。始点座標として使う
   * @param InOriginRotation 探知範囲の形状の回転（ワールド）
   * @param InOriginScale3D  探知範囲の形状のスケール（ワールド）
   * @param OutResult        探知範囲内にあるアクターポインターを保存する配列
   * 
   * @return                 探知範囲内にあるアクターの数 
   */
  UE_API virtual int32 DetectTargets(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, TArray<AActor*>& OutResult) const PURE_VIRTUAL(UPrimitiveDetectorData::DetectTargets, return 0;)

/**Editor debug draw functions */
/**エディタデバッグ描画関数群 */
#if WITH_EDITOR

  public:
    UE_API void DebugDrawRange(const UObject* InWorldContextObject, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const;
    UE_API virtual void DebugDrawRange(class FPrimitiveDrawInterface* PDI, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const { }
    UE_API virtual void DebugDrawRange(class ULineBatchComponent* LineBatch, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const { }

  private:
    UE_API virtual void DebugDrawRange(const UWorld* InWorld, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const { }

#endif
};

#undef UE_API
