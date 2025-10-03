#pragma once

#ifndef _INTERNAL_AR_RANGE_DETECTOR_FILTER_
#define _INTERNAL_AR_RANGE_DETECTOR_FILTER_

#define UE_API ARRANGER_API

struct FRangeDetectorEvaluationResult;

namespace ARRanger
{

namespace Detector
{
  class FRangeDetectorFilter
  {
    protected:
      struct ProtectedToken {};

    public:
      static UE_API TSharedPtr<FRangeDetectorFilter> MakeInstance(UClass* InFilterClass);

      UE_API FRangeDetectorFilter(ProtectedToken, UClass* InFilterClass);
      UE_API virtual ~FRangeDetectorFilter();

      UE_API virtual void ApplyFilter(FRangeDetectorEvaluationResult& OutResult);
      
      bool IsFilterOf(UClass* InFilterClass) const { return FilterClass == InFilterClass; }

      bool IsValid() const { return FilterClass != nullptr; }


    protected:
      TSubclassOf<UObject> FilterClass = nullptr;
  };

  class FRangeDetectorFilter_Interface final : public FRangeDetectorFilter
  {
    public:
      static UE_API TSharedPtr<FRangeDetectorFilter_Interface> MakeInstance(UClass* InFilterClass);

      UE_API FRangeDetectorFilter_Interface(ProtectedToken, UClass* InFilterClass);
      UE_API void ApplyFilter(FRangeDetectorEvaluationResult& OutResult) override final;
  };

} // namespace ARRanger::Detector

} // namespace ARRanger

#undef UE_API

#endif // _INTERNAL_AR_RANGE_DETECTOR_FILTER_