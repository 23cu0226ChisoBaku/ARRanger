// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "RangeDetector/Core/DetectorImpls.h"
#include "GameplayTagContainer.h"

#include <type_traits>

#include "PrimitiveDetectorData.generated.h"

class UWorld;

#define UE_API ARRANGER_API

/**
 * Definition of interface
 * use this inside derived class (.h file)
 */
#define DECLARE_PRIMITIVE_DETECTOR(DetectorType) \
  virtual int32 DetectTargets(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, TArray<AActor*>& OutResult) const override;\

/**
 * Declarations of DECLARE_PRIMITIVE_DETECTOR
 * use this for derived class (.cpp file)
 */
#define DEFINE_PRIMITIVE_DETECTOR(DetectorType) \
  int32 DetectorType::DetectTargets(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, TArray<AActor*>& OutResult) const \
  { \
    static_assert(std::is_base_of_v<UPrimitiveDetectorData, DetectorType>, "Invalid type, Use inside DERIVED class of UPrimitiveDetectorData"); \
    check(World != nullptr); \
    return DetectTargetsProtected<decltype(*this)>(World, OriginActor, InOriginLocation, InOriginRotation, InOriginScale3D, OutResult); \
  }
  
UCLASS(Abstract, Const)
class UPrimitiveDetectorData : public UDataAsset
{
	GENERATED_BODY()
	
public:

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|DetectorData")
  FGameplayTag DataTag;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|DetectorData")
  FVector CenterPositionOffset;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|DetectorData")
  FRotator RotationOffset;

#if WITH_EDITORONLY_DATA

  UPROPERTY(EditDefaultsOnly, Category = "Debug")
  FLinearColor DebugLineColor = FLinearColor::Red;
#endif

  /**
   * Interface to access this class
   * Use this to override DetectTargets in derived class
   */
  UE_API virtual int32 DetectTargets(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, TArray<AActor*>& OutResult) const PURE_VIRTUAL(UPrimitiveDetectorData::DetectTargets, return 0;)

protected:
  template<typename DetectorDataType = UPrimitiveDetectorData>
  int32 DetectTargetsProtected(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, TArray<AActor*>& OutResult) const
  {
    static_assert(!std::is_base_of_v<UPrimitiveDetectorData, DetectorDataType>, "Invalid type, Use DERIVED class of UPrimitiveDetectorData");
    return ARRanger::Detector::DetectTargetsImpl(World, OriginActor, InOriginLocation, InOriginRotation, InOriginScale3D, static_cast<const DetectorDataType&>(*this), OutResult);
  }

#if WITH_EDITOR

  public:
    UE_API void DebugDrawRange(const UObject* InWorldContextObject, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const;

    UE_API virtual void DebugDrawRange(class FPrimitiveDrawInterface* PDI, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const { }

    UE_API virtual void DebugDrawRange(class ULineBatchComponent* LineBatch, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const { }

  private:
    UE_API virtual void DebugDrawRange(const UWorld* InWorld, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const { }

#endif
};

#undef UE_API
