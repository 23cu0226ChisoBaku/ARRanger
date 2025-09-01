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
};

struct FRangeDetectorEvaluationResult
{
  TArray<TObjectPtr<AActor>> DetectedActors{};
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
      ARRANGER_API FString GetDataTagName() const;
      ARRANGER_API int32 EvaluateDetector(const FRangeDetectorEvaluationParameter& EvaluationParam, FRangeDetectorEvaluationResult& OutResult) const;

      const UPrimitiveDetectorData* GetData_Const() const { return m_constData.Get(); }
      int32 GetPriority() const { return m_priority; }
      bool IsActivate() const { return bIsActivated; }

#if WITH_EDITOR

      void DebugDrawRange(const FVector& InOriginPosition, const FColor& InLineColor);
      
#endif // WITH_EDITOR

    private:
      TWeakObjectPtr<const UPrimitiveDetectorData> m_constData;

      int32 m_priority;

      uint8 bIsActivated : 1;

    public:
      FRangeDetector(const FRangeDetector&) = delete;
      FRangeDetector& operator=(const FRangeDetector&) = delete;
      FRangeDetector(FRangeDetector&&) = delete;
      FRangeDetector& operator=(FRangeDetector&&) = delete;
  };
}

}

#endif // _AR_RANGE_DETECTOR_