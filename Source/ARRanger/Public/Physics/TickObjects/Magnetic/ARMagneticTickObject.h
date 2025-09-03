// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Physics/Core/ARPhysicsTickObject.h"

#include "ARMagneticTickObject.generated.h"

class IARMagnetizableInterface;
class FARPhysicsEngineProxy;

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UARMagneticTickObject : public UARPhysicsTickObject
{
	GENERATED_BODY()

  public:
    ARRANGER_API UARMagneticTickObject();
    ARRANGER_API void SetTargetObject(IARMagnetizableInterface* InTarget);
    ARRANGER_API void SetPhysicsEngineProxy(FARPhysicsEngineProxy* InPhysicsEngineProxy);
    ARRANGER_API void RegisterAffectedMagnetizedObject(IARMagnetizableInterface* MagnetizedObject);
    bool IsSameTarget(const IARMagnetizableInterface* InTarget) const { return Target == InTarget; }
    bool IsTargetValid() const { return Target != nullptr; } 

  protected:
    /**Start UARPhysicsTickObject interface */
    ARRANGER_API virtual void OnEndTickObject() override;
    /**End UARPhysicsTickObject interface */

  protected:
    IARMagnetizableInterface* Target;
    TArray<IARMagnetizableInterface*> AffectedMagnetizedObjects;
    FARPhysicsEngineProxy* PhysicsEngineProxy;

  private:

};

