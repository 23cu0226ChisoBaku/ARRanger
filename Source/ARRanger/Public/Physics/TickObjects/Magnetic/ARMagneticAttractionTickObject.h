// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Physics/TickObjects/Magnetic/ARMagneticTickObject.h"

#include "ARMagneticAttractionTickObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UARMagneticAttractionTickObject : public UARMagneticTickObject
{
	GENERATED_BODY()

  protected:      
    ARRANGER_API virtual void OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result) override;
};
