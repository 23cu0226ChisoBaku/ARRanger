/**
 * @file RangeDetectorFilter.h
 * @author MAI ZHICONG
 * @brief 範囲探知クラス用フィルタークラス
 */

#pragma once

#ifndef _INTERNAL_AR_RANGE_DETECTOR_FILTER_
#define _INTERNAL_AR_RANGE_DETECTOR_FILTER_

#define UE_API ARRANGER_API

// 前方宣言 
struct FRangeDetectorEvaluationResult;

namespace ARRanger
{

namespace Detector
{
  /**
   * @brief 範囲探知クラス用フィルタークラス(UInterface以外)
   */
  class FRangeDetectorFilter
  {
    protected:
      // 外部がインスタンスを生成できないようなToken
      // MakeInstance経由でインスタンス作成する必要がある
      enum class ProtectedToken {};

    public:

      /**
       * @brief フィルターインスタンス作成
       */
      static UE_API TSharedPtr<FRangeDetectorFilter> MakeInstance(UClass* InFilterClass);

      UE_API FRangeDetectorFilter(ProtectedToken, UClass* InFilterClass);
      UE_API virtual ~FRangeDetectorFilter();

      /**
       * @brief フィルターを適用する
       * @param OutResult （OUT）適用対象
       */
      UE_API virtual void ApplyFilter(FRangeDetectorEvaluationResult& OutResult);
      
      bool IsFilterOf(UClass* InFilterClass) const { return FilterClass == InFilterClass; }
      bool IsValid() const { return FilterClass != nullptr; }

    protected:
      TSubclassOf<UObject> FilterClass = nullptr;
  };

  /**
   * @brief 範囲探知クラス用フィルタークラス(UInterface専用)
   */
  class FRangeDetectorFilter_Interface final : public FRangeDetectorFilter
  {
    public:
      /**
       * @brief フィルターインスタンス作成
       */
      static UE_API TSharedPtr<FRangeDetectorFilter_Interface> MakeInstance(UClass* InFilterClass);

      UE_API FRangeDetectorFilter_Interface(ProtectedToken, UClass* InFilterClass);

      /**Start FRangeDetectorFilter Interface */
      UE_API void ApplyFilter(FRangeDetectorEvaluationResult& OutResult) override final;
      /**End FRangeDetectorFilter Interface */
  };

} // namespace ARRanger::Detector

} // namespace ARRanger

#undef UE_API

#endif // _INTERNAL_AR_RANGE_DETECTOR_FILTER_