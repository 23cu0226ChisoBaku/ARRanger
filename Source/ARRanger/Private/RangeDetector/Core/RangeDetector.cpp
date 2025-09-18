#include "RangeDetector/Core/RangeDetector.h"
#include "RangeDetector/Core/PrimitiveDetectorData.h"

namespace
{
  void FilterActorByClass(TArray<AActor*>& OutResult, TSubclassOf<AActor> ActorClass);
  void FilterActorByInterface(TArray<AActor*>& OutResult, TSubclassOf<UInterface> InterfaceClass);
}


void FRangeDetectorEvaluationResult::Reset()
{
  DetectedActors.Reset();
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

  void FRangeDetector::SetFilter(const FRangeDetectorFilterData& InFilterData)
  {
    if (!InFilterData.IsValid())
    {
      return;
    }

    m_filter = InFilterData;
  }

  void FRangeDetector::RemoveFilter()
  {
    m_filter = FRangeDetectorFilterData{};
  }

  FString FRangeDetector::GetDataTagName() const
  {
    return m_constData != nullptr ? m_constData->DataTag.GetTagName().ToString() : FString{};
  }

  int32 FRangeDetector::EvaluateDetector(const FRangeDetectorEvaluationParameter& EvaluationParam)
  {
    if (!IsActivate() || (m_constData == nullptr))
    {
      UE_LOG(LogTemp, Warning, TEXT("Detector is invalid."));
      return 0;
    }

    m_evaluatedResult.Reset();
    
    if (EvaluationParam.OriginSceneComp != nullptr)
    {
      (void)m_constData->DetectTargets(
                                        EvaluationParam.World, 
                                        EvaluationParam.OriginActor,  
                                        EvaluationParam.OriginSceneComp->GetComponentLocation(),
                                        EvaluationParam.OriginSceneComp->GetComponentRotation(),
                                        EvaluationParam.OriginSceneComp->GetComponentScale(),
                                        m_evaluatedResult.DetectedActors
                                      );
    }


    FilterResult();

    return m_evaluatedResult.DetectedActors.Num();
  }


  
  void FRangeDetector::DebugDrawRange(USceneComponent* InOriginSceneComp)
  {
    #if WITH_EDITOR

    if (InOriginSceneComp == nullptr)
    {
      return;
    }
    
    if (m_constData.IsValid())
    {
      m_constData->DebugDrawRange (
                                  InOriginSceneComp, 
                                  InOriginSceneComp->GetComponentLocation(),
                                  InOriginSceneComp->GetComponentRotation(),
                                  InOriginSceneComp->GetComponentScale()
                                  );
    }
    
    #endif // WITH_EDITOR
  }
      

  void FRangeDetector::FilterResult()
  {
    if (m_filter.IsValid())
    {
      using enum ERangeDetectorFilterType;
      switch (m_filter.FilterType)
      {
        case RDF_Actor:
        {
          FilterActorByClass(m_evaluatedResult.DetectedActors, m_filter.FilterClass);
        }
        break;
        case RDF_Interface:
        {
          FilterActorByInterface(m_evaluatedResult.DetectedActors, m_filter.FilterClass);
        }
        break;

        // Mark default as an exception
        default:
        {
          check(false);
        }
        break;
      }
    }
  }

} // namespace ARRanger::Detector

} // namespace ARRanger


namespace
{
  void FilterActorByClass(TArray<AActor*>& OutResult, TSubclassOf<AActor> ActorClass)
  {
    if (ActorClass == nullptr)
    {
      return;
    }

    int32 idx = 0;
    while (idx < OutResult.Num())
    {
      AActor* actor = OutResult[idx];
      // Remove invalid actor or actor that is not implemented ActorClass
      if ((actor == nullptr) || !actor->GetClass()->IsChildOf(ActorClass))
      {
        OutResult.RemoveAt(idx);
        continue;
      }

      idx++;
    }
  }

  void FilterActorByInterface(TArray<AActor*>& OutResult, TSubclassOf<UInterface> InterfaceClass)
  {
    if (InterfaceClass == nullptr)
    {
      return;
    }

    int32 idx = 0;
    while (idx < OutResult.Num())
    {
      AActor* actor = OutResult[idx];
      // Remove invalid actor or actor that is not implemented InterfaceClass
      if ((actor == nullptr) || !actor->GetClass()->ImplementsInterface(InterfaceClass))
      {
        OutResult.RemoveAt(idx);
        continue;
      }

      idx++;
    }
  }
}

