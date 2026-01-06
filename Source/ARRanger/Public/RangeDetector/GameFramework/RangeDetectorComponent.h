#pragma once

#include "Components/SceneComponent.h"
#include "RangeDetector/DetectorTypes.h"

#include "RangeDetectorComponent.generated.h"

// 前方宣言
struct FRangeDetectorEvaluationResult;
class UPrimitiveDetectorData;
namespace ARRanger
{
namespace Detector
{
  // class ARRanger::Detector::FRangeDetector
  class FRangeDetector;
} // namespace ARRanger::Detector
} // namespace ARRanger

#define UE_API ARRANGER_API

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class URangeDetectorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UE_API URangeDetectorComponent();

protected:
	// Called when the game starts
	UE_API virtual void BeginPlay() override;

public:	
  /**Start UActorComponent Interface */
	UE_API virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  UE_API virtual void OnUnregister() override;
  /**End UActorComponent Interface */

  UE_API void AddNewDetector(const FDetectorAssetEntry& Entry);

  UE_API void RemoveDetector(const UPrimitiveDetectorData* RangeData);

  UE_API bool HasRangeData(const UPrimitiveDetectorData* RangeData);

  UE_API int32 GetResultByRangeData(const UPrimitiveDetectorData* RangeData, FRangeDetectorEvaluationResult& OutResult) const;

  UE_API bool IsDetectorEmpty() const;

// Only for FComponentVisualizer Use
#if WITH_EDITOR

  UE_API void ED_DrawComponentVisualizer(class FPrimitiveDrawInterface* PDI) const;

  UE_API void ED_DrawWithLineBatchComp(class ULineBatchComponent* LineBatch) const;
  
#endif 

private:
  void AddFilterInternal(ARRanger::Detector::FRangeDetector& OutRangeDetectorRef, const FDetectorTargetInfo& TargetInfo);

private:

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector", meta = (DisplayName = "DataAssetEntry", AllowPrivateAccess = "true"))
  TArray<FDetectorAssetEntry> DetectorAssetEntries;

  TArray< TPimplPtr< ARRanger::Detector::FRangeDetector > > m_rangeDetectorInsts;
  
public:

#if WITH_EDITORONLY_DATA
  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Debug", Transient)
  bool bDrawDebugRange = false;
#endif // WITH_EDITORONLY_DATA
  
  uint8 bStopWhenOwnerDestroyed : 1;
};

#undef UE_API