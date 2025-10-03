#include "RangeDetectorFilter.h"

#include "RangeDetector/Core/RangeDetector.h"

namespace ARRanger
{

namespace Detector
{

TSharedPtr<FRangeDetectorFilter> FRangeDetectorFilter::MakeInstance(UClass* InFilterClass)
{
  return ::MakeShared<ARRanger::Detector::FRangeDetectorFilter>(ProtectedToken{}, InFilterClass);
}

FRangeDetectorFilter::FRangeDetectorFilter(ProtectedToken, UClass* InFilterClass)
  : FilterClass{InFilterClass}
{ }

FRangeDetectorFilter::~FRangeDetectorFilter() = default;

void FRangeDetectorFilter::ApplyFilter(FRangeDetectorEvaluationResult& OutResult)
{
  if (!IsValid())
  {
    return;
  }

  TArray<AActor*>& outResultArray = OutResult.DetectedActors;

  int32 idx = 0;    
  while (idx < outResultArray.Num())
  {
    AActor* actor = outResultArray[idx];
    // Remove invalid actor or actor that is not implemented ActorClass
    if ((actor == nullptr) || !actor->GetClass()->IsChildOf(FilterClass))
    {
      outResultArray.RemoveAt(idx);
      continue;
    }

    idx++;
  }
}

TSharedPtr<FRangeDetectorFilter_Interface> FRangeDetectorFilter_Interface::MakeInstance(UClass* InFilterClass)
{
  return ::MakeShared<ARRanger::Detector::FRangeDetectorFilter_Interface>(ProtectedToken{}, InFilterClass);
}

FRangeDetectorFilter_Interface::FRangeDetectorFilter_Interface(ProtectedToken, UClass* InFilterClass)
  : FRangeDetectorFilter(ProtectedToken{}, InFilterClass)
{ }

void FRangeDetectorFilter_Interface::ApplyFilter(FRangeDetectorEvaluationResult& OutResult)
{
  if (!IsValid())
  {
    return;
  }

  TArray<AActor*>& outResultArray = OutResult.DetectedActors;

  int32 idx = 0;    
  while (idx < outResultArray.Num())
  {
    AActor* actor = outResultArray[idx];
    // Remove invalid actor or actor that is not implemented ActorClass
    if ((actor == nullptr) || !actor->GetClass()->ImplementsInterface(FilterClass))
    {
      outResultArray.RemoveAt(idx);
      continue;
    }

    idx++;
  }
}

}

}