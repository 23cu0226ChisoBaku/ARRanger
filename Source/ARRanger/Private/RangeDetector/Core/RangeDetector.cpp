#include "RangeDetector/Core/RangeDetector.h"
#include "RangeDetector/Core/PrimitiveDetectorData.h"

namespace ARRanger
{

namespace Detector
{
  FRangeDetector::FRangeDetector(UPrimitiveDetectorData* InData, int32 InPriority)
    : m_constData(InData)
    , m_priority(InPriority)
    , bIsActivated(false)
  {
    check(InData != nullptr);
  }

  FRangeDetector::~FRangeDetector() = default;
  
  void FRangeDetector::Enable()
  {
    if (IsActivate())
    {
      return;
    }

    bIsActivated = true;
  }

  void FRangeDetector::Disable()
  {
    if (!IsActivate())
    {
      return;
    }

    bIsActivated = false;
  }

  FString FRangeDetector::GetDataTagName() const
  {
    return m_constData != nullptr ? m_constData->DataTag.GetTagName().ToString() : FString{};
  }

  int32 FRangeDetector::EvaluateDetector(const FRangeDetectorEvaluationParameter& EvaluationParam, FRangeDetectorEvaluationResult& OutResult) const
  {
    if (!IsActivate() || (m_constData == nullptr))
    {
      UE_LOG(LogTemp, Warning, TEXT("Detector is invalid."));
      return 0;
    }

    return m_constData->DetectTargets(EvaluationParam.World, EvaluationParam.OriginActor, OutResult.DetectedActors);
  }


#if WITH_EDITOR

  void FRangeDetector::DebugDrawRange(const FVector& InOriginPosition, const FColor& InLineColor)
  {
    if (m_constData.IsValid())
    {
      m_constData->DebugDrawRange(InOriginPosition, InLineColor);
    }
  }
      
#endif // WITH_EDITOR

}

}