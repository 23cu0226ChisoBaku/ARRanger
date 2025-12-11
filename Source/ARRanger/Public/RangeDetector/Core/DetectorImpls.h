/**
 * @file DetectorImpls.h
 * @brief DetectTargets implementation function's declarations
 */
/**
 * @brief 範囲探知実装関数群の宣言
 */

#pragma once

#ifndef _AR_RANGE_DETECTOR_IMPLS_
#define _AR_RANGE_DETECTOR_IMPLS_

/**Forward declaration */
class UPrimitiveDetectorData;
class UConeDetectorData;
class UCapsuleDetectorData;
class USphereDetectorData;

#define UE_API ARRANGER_API

/**
 * @brief Implementation function declaration macro
 */
/**
 * @brief 実装関数宣言マクロ
 */
#define DECLARE_DETECT_TARGETS_IMPL(DetectorDataType) \
  extern UE_API int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, const DetectorDataType& InData, TArray<AActor*>& OutResult);

namespace ARRanger
{

namespace Detector
{

  DECLARE_DETECT_TARGETS_IMPL(UPrimitiveDetectorData);
  DECLARE_DETECT_TARGETS_IMPL(UConeDetectorData);
  DECLARE_DETECT_TARGETS_IMPL(UCapsuleDetectorData);
  DECLARE_DETECT_TARGETS_IMPL(USphereDetectorData);

} // namespace ARRanger::Detector

} // namespace ARRanger

#undef UE_API

#endif // _AR_RANGE_DETECTOR_IMPLS_