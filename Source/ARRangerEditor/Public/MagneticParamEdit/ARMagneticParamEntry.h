// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"

#include "ARMagneticParamEntry.generated.h"

UCLASS(Blueprintable, BlueprintType, Const)
class UARMagneticParamEntry : public UObject
{
	GENERATED_BODY()

public:

  UPROPERTY(EditDefaultsOnly)
  FName TargetClassName;

  UPROPERTY(EditDefaultsOnly)
  float TargetMass;

  UPROPERTY(EditDefaultsOnly)
  float TargetMagneticPoleStrength;
	
};
