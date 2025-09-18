#pragma once

#ifndef _AR_RANGE_DETECTOR_IMPLS_
#define _AR_RANGE_DETECTOR_IMPLS_

class UPrimitiveDetectorData;
class UConeCollisionDataAsset;
class UCapsuleDetectorData;
class USphereDetectorData;

#define UE_API ARRANGER_API

#define DECLARE_DETECT_TARGETS_IMPL(DetectorDataType) \
  extern UE_API int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, const DetectorDataType& InData, TArray<AActor*>& OutResult);

namespace ARRanger
{

namespace Detector
{
  DECLARE_DETECT_TARGETS_IMPL(UPrimitiveDetectorData);
  DECLARE_DETECT_TARGETS_IMPL(UConeCollisionDataAsset);
  DECLARE_DETECT_TARGETS_IMPL(UCapsuleDetectorData);
  DECLARE_DETECT_TARGETS_IMPL(USphereDetectorData);
} // namespace ARRanger::Detector

} // namespace ARRanger

#undef UE_API

#endif // _AR_RANGE_DETECTOR_IMPLS_