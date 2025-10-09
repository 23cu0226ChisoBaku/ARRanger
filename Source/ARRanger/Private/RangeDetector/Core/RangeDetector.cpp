#include "RangeDetector/Core/RangeDetector.h"
#include "RangeDetector/Core/PrimitiveDetectorData.h"

#include "Internal/RangeDetector/Core/RangeDetectorFilter.h"
#include "Internal/ARLoggingHeader.h"

int32 FRangeDetectorEvaluationResult::GetResultNum() const
{
  // TODO Currently we using DetectedActors ONLY.
  // FIXME Change to HitResults to keep more information
  return DetectedActors.Num();
}

void FRangeDetectorEvaluationResult::Reset()
{
  DetectedActors.Reset();
  HitResults.Reset();
}

bool FRangeDetectorFilterData::IsValid() const
{
  return (FilterType != ERangeDetectorFilterType::RDF_MaxNum) && 
         (FilterClass != nullptr);
}

namespace ARRanger
{

namespace Detector
{
  FRangeDetector::FRangeDetector(const UPrimitiveDetectorData& InData, int32 InPriority)
    : m_constData(&InData)
    , m_priority(InPriority)
    , bIsActivated(false)
  { }

  FRangeDetector::~FRangeDetector() = default;

  void FRangeDetector::SetEnable(bool bEnable)
  {
    bIsActivated = bEnable;
  }

  void FRangeDetector::AddFilter(FRangeDetectorFilterData&& InFilterData)
  {
    if (!InFilterData.IsValid())
    {
      return;
    }

    TSharedPtr<ARRanger::Detector::FRangeDetectorFilter> newFilter{};

    switch (InFilterData.FilterType)
    {
      case ERangeDetectorFilterType::RDF_Actor:
      {
        newFilter = ARRanger::Detector::FRangeDetectorFilter::MakeInstance(InFilterData.FilterClass);
      }
      break;

      case ERangeDetectorFilterType::RDF_Interface:
      {
        newFilter = ARRanger::Detector::FRangeDetectorFilter_Interface::MakeInstance(InFilterData.FilterClass);
      }
      break;
    }

    m_filters.Emplace(newFilter);
  }

  void FRangeDetector::RemoveFilter(UClass* InFilterClass)
  {
    int32 idx = 0;
    while (idx < m_filters.Num())
    {
      const auto& filter = m_filters[idx];
      if (filter.IsValid() && filter->IsFilterOf(InFilterClass))
      {
        m_filters.RemoveAt(idx);
        continue;
      }

      ++idx;
    }
  }

  void FRangeDetector::RemoveAllFilters()
  {
    m_filters.Reset();
  }

  FString FRangeDetector::GetDataTagString() const
  {
    return m_constData != nullptr ? m_constData->DataTag.ToString() : TEXT("Invalid");
  }

  int32 FRangeDetector::Evaluate(const FRangeDetectorEvaluationParameter& EvaluationParam)
  {
    if (!IsActivate())
    {
      UE_LOG(LogARRangeDetector, Warning, TEXT("Range detector is not activated"));
      return 0;
    }
    else if (!m_constData.IsValid())
    {
      UE_LOG(LogARRangeDetector, Error, TEXT("Range detector is invalid.Remove it immediately"));
      return 0;
    }

    m_evaluatedResult.Reset();
    
    if (EvaluationParam.OriginSceneComp != nullptr)
    {
      TArray<AActor*> detectResults{};
      const FVector originLocation = EvaluationParam.OriginSceneComp->GetComponentLocation();
      const FRotator originRotation = EvaluationParam.OriginSceneComp->GetComponentRotation();
      const FVector originScale3D = EvaluationParam.OriginSceneComp->GetComponentScale();

      (void)m_constData->DetectTargets(
                                        EvaluationParam.World, 
                                        EvaluationParam.OriginActor,  
                                        originLocation,
                                        originRotation,
                                        originScale3D,
                                        detectResults
                                      );
      
      m_evaluatedResult.DetectedActors.Append(detectResults);
    }

    CheckFilterValidation();
    FilterResult();

    return m_evaluatedResult.GetResultNum();
  }
  
#if WITH_EDITOR
  void FRangeDetector::DebugDrawRange(USceneComponent* InOriginSceneComp)
  {

    if (InOriginSceneComp == nullptr)
    {
      return;
    }
    
    if (m_constData.IsValid())
    {
      const FVector originLocation = InOriginSceneComp->GetComponentLocation();
      const FRotator originRotation = InOriginSceneComp->GetComponentRotation();
      const FVector originScale3D = InOriginSceneComp->GetComponentScale();

      m_constData->DebugDrawRange (
                                    InOriginSceneComp, 
                                    originLocation,
                                    originRotation,
                                    originScale3D
                                  );
    }
  } 
#endif // WITH_EDITOR
      
  void FRangeDetector::FilterResult()
  {
    for (const auto& filter : m_filters)
    {
      if (filter.IsValid())
      {
        filter->ApplyFilter(m_evaluatedResult);
      }
    }
  }

  void FRangeDetector::CheckFilterValidation()
  {
    int32 idx = 0;
    while (idx < m_filters.Num())
    {
      const auto& filter = m_filters[idx];
      if (!filter.IsValid() || !filter->IsValid())
      {
        m_filters.RemoveAt(idx);
        continue;
      }

      ++idx;
    }
  }

} // namespace ARRanger::Detector

} // namespace ARRanger


