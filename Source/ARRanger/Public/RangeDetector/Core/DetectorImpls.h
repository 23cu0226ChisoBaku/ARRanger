#pragma once

#ifndef _AR_RANGE_DETECTOR_IMPLS_
#define _AR_RANGE_DETECTOR_IMPLS_

class UPrimitiveDetectorData;
class UConeCollisionDataAsset;


#define DECLARE_DETECT_TARGETS_IMPL(DetectorDataType) \
  extern ARRANGER_API int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const DetectorDataType& InData, TArray<TObjectPtr<AActor>>& OutResult);

namespace ARRanger
{

namespace Detector
{
  DECLARE_DETECT_TARGETS_IMPL(UPrimitiveDetectorData);
  DECLARE_DETECT_TARGETS_IMPL(UConeCollisionDataAsset);
}

}

#endif // _AR_RANGE_DETECTOR_IMPLS_