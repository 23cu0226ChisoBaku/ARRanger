// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "RangeDetector/Core/DetectorImpls.h"
#include "GameplayTagContainer.h"

#include <type_traits>

#include "PrimitiveDetectorData.generated.h"


/**
 * Definition of interface
 * use this inside derived class (.h file)
 */
#define DECLARE_PRIMITIVE_DETECTOR(DetectorType) \
  virtual int32 DetectTargets(TArray<AActor*>& OutResult) const override;\

/**
 * Declarations of DECLARE_PRIMITIVE_DETECTOR
 * use this for derived class (.cpp file)
 */
#define DEFINE_PRIMITIVE_DETECTOR(DetectorType) \
  int32 DetectorType::DetectTargets(TArray<AActor*>& OutResult) const \
  { \
    static_assert(std::is_base_of_v<UPrimitiveDetectorData, DetectorType>, "Invalid type, Use inside DERIVED class of UPrimitiveDetectorData"); \
    return DetectTargetsProtected<decltype(*this)>(OutResult); \
  }
  
UCLASS(Abstract, Const)
class UPrimitiveDetectorData : public UDataAsset
{
	GENERATED_BODY()
	
public:

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|DetectorData")
  FGameplayTag DataTag;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|DetectorData")
  FVector CenterPosition;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|DetectorData")
  FVector CenterPositionOffset;

  /**
   * Interface to access this class
   * Use this to override DetectTargets in derived class
   */
  ARRANGER_API virtual int32 DetectTargets(TArray<AActor*>& OutResult) const PURE_VIRTUAL(UPrimitiveDetectorData::DetectTargets, return 0;)

protected:
  template<typename DetectorDataType = UPrimitiveDetectorData>
  int32 DetectTargetsProtected(TArray<AActor*>& OutResult) const
  {
    static_assert(!std::is_base_of_v<UPrimitiveDetectorData, DetectorDataType>, "Invalid type, Use DERIVED class of UPrimitiveDetectorData");
    return ARRanger::Detector::DetectTargetsImpl(static_cast<const DetectorDataType&>(*this), OutResult);
  }

#if WITH_EDITOR

  public:
    ARRANGER_API void DebugDrawRange(const FVector& InOriginPosition, const FColor& InColor) const;

  private:
    ARRANGER_API virtual void DebugDrawRange(const UWorld* InWorld, const FVector& InOriginPosition, const FColor& InColor) const { }

#endif
};
