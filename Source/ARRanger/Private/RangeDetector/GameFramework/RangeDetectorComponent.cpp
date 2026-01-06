#include "RangeDetector/GameFramework/RangeDetectorComponent.h"

#include "RangeDetector/Core/RangeDetector.h"
#include "RangeDetector/Core/PrimitiveDetectorData.h"

#if WITH_EDITOR
#include "SceneManagement.h"                // Use of FPrimitiveDrawInterface
#include "Components/LineBatchComponent.h"
#endif // WITH_EDITOR

URangeDetectorComponent::URangeDetectorComponent()
  : bStopWhenOwnerDestroyed{true}
{
  PrimaryComponentTick.bCanEverTick = true;
  PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void URangeDetectorComponent::BeginPlay()
{
  Super::BeginPlay();

  for (const auto& entry : DetectorAssetEntries)
  {
    AddNewDetector(entry);
  }
}

void URangeDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  FRangeDetectorEvaluationParameter evaluationParam
  {
    .World = GetWorld(),
    .OriginActor = GetOwner(),
    .OriginSceneComp = this  
  };

  for (int32 idx = 0; idx < m_rangeDetectorInsts.Num(); ++idx)
  {
    const TPimplPtr< ARRanger::Detector::FRangeDetector >& detector = m_rangeDetectorInsts[idx];
    if (detector.IsValid())
    {
      detector->Evaluate(evaluationParam);

#if WITH_EDITOR
      const FRangeDetectorEvaluationResult& result = detector->GetEvaluatedResult();
      if (result.DetectedActors.Num() > 0)
      {
        UE_LOG(LogTemp, Error, TEXT("Hit something!!!! Hit count: [%d]"), result.DetectedActors.Num());
      }
#endif

#if WITH_EDITORONLY_DATA 
      if (bDrawDebugRange)
      {
        if (detector->GetData_Const() != nullptr)
        {
          detector->GetData_Const()->DebugDrawRange(this, GetComponentLocation(), GetComponentRotation(), GetComponentScale());
        }
      }
#endif 
    }
  }

}

void URangeDetectorComponent::OnUnregister()
{
  if ((GetOwner() == nullptr) || bStopWhenOwnerDestroyed)
  {
    // TODO Do something?
  }

  m_rangeDetectorInsts.Reset();

  Super::OnUnregister();
}


void URangeDetectorComponent::AddNewDetector(const FDetectorAssetEntry& Entry)
{
  if (Entry.DetectorData == nullptr)
  {
    UE_LOG(LogTemp, Error, TEXT("URangeDetectorComponent::AddNewDetector encountered ERROR.Detector Data is invalid"));
    return;
  }

  TPimplPtr< ARRanger::Detector::FRangeDetector > detectorInst = 
    ::MakePimpl< ARRanger::Detector::FRangeDetector >(*Entry.DetectorData, Entry.Priority);

  AddFilterInternal(*detectorInst, Entry.TargetInfo);
  detectorInst->SetEnable(true);

  m_rangeDetectorInsts.Emplace(::MoveTemp(detectorInst));
} 

void URangeDetectorComponent::RemoveDetector(const UPrimitiveDetectorData* RangeData)
{
  if (RangeData == nullptr)
  {
    return;
  }

  for (int32 idx = 0; idx < m_rangeDetectorInsts.Num(); ++idx)
  {
    const auto& detectorInst = m_rangeDetectorInsts[idx];
    if (detectorInst.IsValid())
    {
      if (detectorInst->GetData_Const() == RangeData)
      {
        m_rangeDetectorInsts.RemoveAt(idx);
        break;
      }
    }
  }
}

int32 URangeDetectorComponent::GetResultByRangeData(const UPrimitiveDetectorData* RangeData, FRangeDetectorEvaluationResult& OutResult) const
{
  OutResult.Reset();
  if (RangeData != nullptr)
  {
    for (const auto& detectorInst : m_rangeDetectorInsts)
    {
      if (detectorInst.IsValid())
      {
        if (detectorInst->GetData_Const() == RangeData)
        {
          OutResult = detectorInst->GetEvaluatedResult();
          return OutResult.DetectedActors.Num();
        }
      }
    }
  }

  return 0;
}


bool URangeDetectorComponent::HasRangeData(const UPrimitiveDetectorData* RangeData)
{
  if (RangeData == nullptr)
  {
    return false;
  }

  for (const auto& detectorInst : m_rangeDetectorInsts)
  {
    if ((detectorInst.IsValid()) && (detectorInst->GetData_Const() == RangeData))
    {
      return true;
    }
  }

  return false;
}

bool URangeDetectorComponent::IsDetectorEmpty() const
{
  return m_rangeDetectorInsts.Num() == 0;
}

void URangeDetectorComponent::AddFilterInternal(ARRanger::Detector::FRangeDetector& OutRangeDetectorRef, const FDetectorTargetInfo& TargetInfo)
{

  FRangeDetectorFilterData filter{};
  filter.Type = TargetInfo.Type;

  switch (TargetInfo.Type)
  {
    case EDetectorTargetType::Actor:
    {
      filter.FilterClass = TargetInfo.TargetActor; 
    }
    break;

    case EDetectorTargetType::Interface:
    {
      filter.FilterClass = TargetInfo.TargetInterface;
    }
    break;
  }

  OutRangeDetectorRef.AddFilter(::MoveTemp(filter));

}

#if WITH_EDITOR

void URangeDetectorComponent::ED_DrawComponentVisualizer(FPrimitiveDrawInterface* PDI) const
{
  // Use detector if BeginPlay was called
  if (HasBegunPlay())
  {
    for (const auto& detectorInst : m_rangeDetectorInsts)
    {
      if (detectorInst.IsValid())
      {
        if (const UPrimitiveDetectorData* data = detectorInst->GetData_Const())
        {
          data->DebugDrawRange(PDI, GetComponentLocation(), GetComponentRotation(), GetComponentScale());
        }
      }
    }
  }
  else
  {
    for (const auto& entry : DetectorAssetEntries)
    {
      if (entry.DetectorData != nullptr)
      {
        entry.DetectorData->DebugDrawRange(PDI, GetComponentLocation(), GetComponentRotation(), GetComponentScale());
      }
    }
  }
}

void URangeDetectorComponent::ED_DrawWithLineBatchComp(ULineBatchComponent* LineBatch) const
{
  if (LineBatch != nullptr)
  {
    if (UWorld* world = LineBatch->GetWorld())
    {
      const EWorldType::Type worldType = world->WorldType;
      // Draw only in Editor(without in PIE)
      if (worldType == EWorldType::Type::EditorPreview ||
          worldType == EWorldType::Type::Editor
        )
      {
        for (const auto& detectorInst : m_rangeDetectorInsts)
        {
          if (detectorInst.IsValid())
          {
            if (const UPrimitiveDetectorData* data = detectorInst->GetData_Const())
            {
              data->DebugDrawRange(LineBatch, GetComponentLocation(), GetComponentRotation(), GetComponentScale());
            }
          }
        }
      }
    }
  }
}
  
#endif 

