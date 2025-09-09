// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RangeDetector/Core/PrimitiveDetectorData.h"

#include "SphereDetectorData.generated.h"

#define UE_API ARRANGER_API

/**
 * 
 */
UCLASS()
class USphereDetectorData : public UPrimitiveDetectorData
{
	GENERATED_BODY()

public:

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|DetectorData")
  float SphereRadius;

  DECLARE_PRIMITIVE_DETECTOR(USphereDetectorData);

#if WITH_EDITOR

private:
  UE_API virtual void DebugDrawRange(const UWorld* InWorld, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const override;

  UE_API virtual void DebugDrawRange(FPrimitiveDrawInterface* PDI, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const override;

  UE_API virtual void DebugDrawRange(ULineBatchComponent* LineBatch, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const override;

#endif // WITH_EDITOR

	
};

#undef UE_API