// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeDetector/GameFramework/RangeDetectorComponent.h"

#include "RangeDetector/Core/RangeDetector.h"
#include "RangeDetector/Core/PrimitiveDetectorData.h"


// Sets default values for this component's properties
URangeDetectorComponent::URangeDetectorComponent()
  : bStopWhenOwnerDestroyed{true}
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URangeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

  for (const auto& entry : DetectorAssetEntries)
  {
    AddNewDetector(entry);
  }
}


// Called every frame
void URangeDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  check(GetWorld() != nullptr);

  FRangeDetectorEvaluationParameter evaluationParam
  {
    .World = GetWorld(),
    .OriginActor = GetOwner()  
  };

	for (int32 idx = 0; idx < m_rangeDetectorInsts.Num(); ++idx)
  {
    const TPimplPtr< ARRanger::Detector::FRangeDetector >& detector = m_rangeDetectorInsts[idx];
    if (detector.IsValid())
    {
      detector->EvaluateDetector(evaluationParam);

      const FRangeDetectorEvaluationResult& result = detector->GetEvaluatedResult();

      // TODO For debug purpose
#if WITH_EDITORONLY_DATA

      if (bDrawDebugRange)
      {
        detector->DebugDrawRange(GetOwner()->GetActorLocation(), FColor::Red);
      }

#endif // WITH_EDITORONLY_DATA
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
    ::MakePimpl< ARRanger::Detector::FRangeDetector >(Entry.DetectorData, Entry.Priority);

  detectorInst->Enable();

  FRangeDetectorFilterData filter{};
  switch (Entry.Target.TargetType)
  {
    case EDetectorTargetType::Actor:
    {
      filter.FilterType = ERangeDetectorFilterType::RDF_Actor;
      filter.FilterClass = Entry.Target.TargetActor; 
    }
    break;

    case EDetectorTargetType::Interface:
    {
      filter.FilterType = ERangeDetectorFilterType::RDF_Interface;
      filter.FilterClass = Entry.Target.TargetInterface;
    }
    break;

    default:
    {

    }
    break;
  }

  detectorInst->SetFilter(filter);

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

