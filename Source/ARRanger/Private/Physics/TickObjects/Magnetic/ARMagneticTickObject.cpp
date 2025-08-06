// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/TickObjects/Magnetic/ARMagneticTickObject.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARMagneticTickObject)

UARMagneticTickObject::UARMagneticTickObject()
  : Target{nullptr}
  , AffectedMagnetizedObjects{}
  , PhysicsEngineProxy{nullptr}
{
  PrimaryPhysicsTick.Frequency = EARPhysicsTickFrequency::TF_Default;
}

void UARMagneticTickObject::SetTargetObject(IARMagnetizableInterface* InTarget)
{
  Target = InTarget;
}

void UARMagneticTickObject::SetPhysicsEngineProxy(FARPhysicsEngineProxy* InPhysicsEngineProxy)
{
  PhysicsEngineProxy = InPhysicsEngineProxy;
}

void UARMagneticTickObject::RegisterAffectedMagnetizedObject(IARMagnetizableInterface* MagnetizedObject)
{
  if (MagnetizedObject == nullptr)
  {
    return;
  }

  AffectedMagnetizedObjects.AddUnique(MagnetizedObject);

}

void UARMagneticTickObject::OnEndTickObject()
{
  Super::OnEndTickObject();

  AffectedMagnetizedObjects.Reset();
}
