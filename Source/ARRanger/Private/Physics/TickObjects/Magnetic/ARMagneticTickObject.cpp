// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/TickObjects/Magnetic/ARMagneticTickObject.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARMagneticTickObject)

UARMagneticTickObject::UARMagneticTickObject()
  : Target{nullptr}
  , AffectedMagnetizedObjects{}
{
  PrimaryPhysicsTick.Frequency = EPhysicsExecuteFrequency::Constantly;
}

void UARMagneticTickObject::RegisterAffectedMagnetizedObject(IARMagnetizableInterface* MagnetizedObject)
{
  if (MagnetizedObject != nullptr)
  {
    AffectedMagnetizedObjects.AddUnique(MagnetizedObject);
  }
}

void UARMagneticTickObject::OnPostTickObject()
{
  Super::OnPostTickObject();

  // フレーム単位で影響するため、終了するとき影響を与えるオブジェクト全部解読する
  AffectedMagnetizedObjects.Reset();
}
