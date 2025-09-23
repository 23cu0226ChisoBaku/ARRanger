#pragma once

#ifndef _AR_RANGE_DETECTOR_
#define _AR_RANGE_DETECTOR_

class UPrimitiveDetectorData;

struct FRangeDetectorEvaluationParameter
{
  /**Game world */
  TObjectPtr<UWorld> World = nullptr;

  /**Origin Actor */
  TObjectPtr<AActor> OriginActor = nullptr;

  /**Origin Scene Component. Maybe nullptr */
  TObjectPtr<USceneComponent> OriginSceneComp = nullptr;
};

struct FRangeDetectorEvaluationResult
{
  TArray<AActor*> DetectedActors{};

  // TODO Currently it is unuse. 
  TArray<FHitResult> HitResults{};

  ARRANGER_API void Reset();
};

enum struct ERangeDetectorFilterType
{
  RDF_Actor,
  RDF_Interface,

  RDF_MaxNum,
};

struct FRangeDetectorFilterData
{
  ERangeDetectorFilterType FilterType = ERangeDetectorFilterType::RDF_MaxNum;

  UClass* FilterClass = nullptr;

  ARRANGER_API bool IsValid() const;
};

namespace ARRanger
{

namespace Detector
{
  class FRangeDetector
  {
    public:
      ARRANGER_API FRangeDetector(UPrimitiveDetectorData* InData, int32 InPriority = -1);
      ARRANGER_API ~FRangeDetector();
      
      ARRANGER_API void Enable();
      ARRANGER_API void Disable();
      ARRANGER_API void SetFilter(const FRangeDetectorFilterData& InFilterData);
      ARRANGER_API void RemoveFilter();
      ARRANGER_API FString GetDataTagName() const;
      ARRANGER_API int32 EvaluateDetector(const FRangeDetectorEvaluationParameter& EvaluationParam);

      const UPrimitiveDetectorData* GetData_Const() const { return m_constData.Get(); }
      int32 GetPriority() const { return m_priority; }
      bool IsActivate() const { return static_cast<bool>(bIsActivated); }
      const FRangeDetectorEvaluationResult& GetEvaluatedResult() const { return m_evaluatedResult; }

      void DebugDrawRange(USceneComponent* InOriginSceneComp);

    private:
      void FilterResult();

    private:
      TWeakObjectPtr<const UPrimitiveDetectorData> m_constData;

      FRangeDetectorEvaluationResult m_evaluatedResult;

      int32 m_priority;

      uint8 bIsActivated : 1;

      FRangeDetectorFilterData m_filter;

    public:
      FRangeDetector(const FRangeDetector&) = delete;
      FRangeDetector& operator=(const FRangeDetector&) = delete;
      FRangeDetector(FRangeDetector&&) = delete;
      FRangeDetector& operator=(FRangeDetector&&) = delete;
  };
}

}

#endif // _AR_RANGE_DETECTOR_