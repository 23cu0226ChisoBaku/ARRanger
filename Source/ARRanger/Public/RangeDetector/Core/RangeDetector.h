/**
 * @file RangeDetector.h
 * @brief 範囲にあるアクターを探知するクラス（UPrimitiveDetectorData使用）
 */

#pragma once

#ifndef _AR_RANGE_DETECTOR_
#define _AR_RANGE_DETECTOR_

#include "RangeDetector/DetectorTypes.h"

// 前方宣言
class UPrimitiveDetectorData;
namespace ARRanger
{
namespace Detector
{
  // class ARRanger::Detector::FRangeDetectorFilter
  class FRangeDetectorFilter;
} // namespace ARRanger::Detector
} // namespace ARRanger

#define UE_API ARRANGER_API

/**
 * @brief 範囲探知クラスを評価するパラメータ構造体
 */
struct FRangeDetectorEvaluationParameter
{
  /**Game world */
  TObjectPtr<UWorld> World = nullptr;

  /**Origin Actor */
  TObjectPtr<AActor> OriginActor = nullptr;

  /**Origin Scene Component.(Use transform as center point of range) */
  /**探知範囲の中心点SceneComponent。SceneComponentのTransformを探知範囲の形状のTransformとして使用する */
  TObjectPtr<USceneComponent> OriginSceneComp = nullptr;
};

/**
 * @brief 探知範囲クラスが評価された結果構造体
 */
struct FRangeDetectorEvaluationResult
{
  TArray<AActor*> DetectedActors{};

  // TODO Currently it is unuse. 
  TArray<FHitResult> HitResults{};

  /**
   * @brief 探知結果の数を返す(評価が完全に済んでいないと0を返す可能性がある)
   * 
   * @return 結果を保存する配列要素数
   */
  UE_API int32 GetResultNum() const;

  /**
   * @brief 結果構造体をリセットする
   */
  UE_API void Reset();
};

/**
 * @brief Data structure of filter used by RangeDetector
 */
struct FRangeDetectorFilterData
{
  TSubclassOf<UObject> FilterClass = nullptr;

  EDetectorTargetType Type = EDetectorTargetType::INVALID;

  /**
   * @brief Check whether filter is valid
   * 
   * @return True if both FilterClass and FilterType are valid, otherwise false
   */
  UE_API bool IsValid() const;
};

namespace ARRanger
{

namespace Detector
{

  /**
   * @brief 範囲探知クラス。EvaluateDetector()を呼び出し、現在のフレームに範囲内のターゲットを探知する
   */
  class FRangeDetector
  {
    public:
      /**
       * @brief コンストラクタ
       * @param InData      探知範囲データ。必ず有効なデータが入る
       * @param InPriority  優先順位。// TODO ※現在は使用されていない
       */
      UE_API FRangeDetector(const UPrimitiveDetectorData& InData, int32 InPriority = -1);

      /**
       * @brief デストラクタ
       */
      UE_API ~FRangeDetector();
      
      /**
       * @brief 範囲探知の有効性を設定する
       * 
       * @param bEnable
       */
      UE_API void SetEnable(bool bEnable);

      /**
       * @brief フィルターを追加する
       * 
       * @param InFilterData @see FRangeDetectorFilterData
       */
      UE_API void AddFilter(FRangeDetectorFilterData&& InFilterData);

      /**
       * @brief UClassが含んだフィルターを外す
       * @param InFilterClass フィルターUClass 
       */
      UE_API void RemoveFilter(UClass* InFilterClass);

      /**
       * @brief フィルターを全て外す
       */
      UE_API void RemoveAllFilters();

      /**
       * @brief UPrimitiveDetectorDataのDataTagをFStringで返す
       * @return DataTag.ToString()。また、UPrimitiveDetectorDataは無効だったら"Invalid"を返す
       */
      UE_API FString GetDataTagString() const;

      /**
       * @brief Evaluate RangeDetector
       * 
       * @param EvaluationParam @see FRangeDetectorEvaluationParameter
       * @return Num of detected target.
       */
      UE_API int32 Evaluate(const FRangeDetectorEvaluationParameter& EvaluationParam);

      __forceinline const UPrimitiveDetectorData* GetData_Const() const { return m_constData.Get(); }
      __forceinline int32 GetPriority() const { return m_priority; }
      __forceinline bool IsActivate() const { return static_cast<bool>(bIsActivated); }
      __forceinline const FRangeDetectorEvaluationResult& GetEvaluatedResult() const { return m_evaluatedResult; }

      /**
       * @brief UPrimitiveDetectorDataを用いてデバッグラインを描画する
       * 
       * @param InOriginSceneComp 探知範囲の中心座標となるSceneComponent
       */
      UE_API void DebugDrawRange(USceneComponent* InOriginSceneComp);

    private:

      /**
       * @brief 評価した結果にフィルターをかける。※評価結果が変わる可能性がある
       */
      void FilterResult();

      void CheckFilterValidation();

    private:

      /**フィルター配列 */
      TArray<TSharedPtr<FRangeDetectorFilter>> m_filters;

      /**探知範囲データポインター（const） */
      TWeakObjectPtr<const UPrimitiveDetectorData> m_constData;
      
      /**評価結果 */
      FRangeDetectorEvaluationResult m_evaluatedResult;

      /**Priority */ // TODO Currently unused
      int32 m_priority = -1;

      /**Activation flag of RangeDetector */
      uint8 bIsActivated : 1;

    /**コピーとムーブを全て禁止する */
    public:
      FRangeDetector(const FRangeDetector&) = delete;
      FRangeDetector& operator=(const FRangeDetector&) = delete;
      FRangeDetector(FRangeDetector&&) = delete;
      FRangeDetector& operator=(FRangeDetector&&) = delete;
  };

} // namespace ARRanger::Detector

} // namespace ARRanger

#undef UE_API

#endif // _AR_RANGE_DETECTOR_