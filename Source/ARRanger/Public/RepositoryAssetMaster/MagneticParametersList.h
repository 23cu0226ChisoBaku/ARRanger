// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "MagneticParametersList.generated.h"

USTRUCT()
struct FMagneticParametersEntry
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly, meta = (MustImplement = "ARMagnetizableInterface"))
  TSubclassOf<AActor> MagneticPhysicsUserClass;

  UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.1f, ClampMax = 1000.0f))
  float MagneticCharge;

  UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.1f, ClampMax = 1000.0f))
  float MegneticObjectMass;
};

/**
 * 
 */
UCLASS()
class ARRANGER_API UMagneticParametersList : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
  UPROPERTY(EditDefaultsOnly)
  TMap<FString, FMagneticParametersEntry> MagneticParameters;

};
