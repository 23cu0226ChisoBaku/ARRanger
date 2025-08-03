// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/Core/ARPhysicsTickTypes.h"

FARPhysicsTickFunctionInterface::FARPhysicsTickFunctionInterface()
  : PhysicsTickType{EARPhysicsTickType::TT_Magnetic}
  , m_tickState{Disabled}
  , bIsRegistered{false}
{
}

FARPhysicsTickFunctionInterface::~FARPhysicsTickFunctionInterface()
{
  UnregisterPhysicsTickFunction();
}

void FARPhysicsTickFunctionInterface::RegisterPhysicsTickFunction()
{
  if (!IsTickFunctionRegistered())
  {
    // TODO Register tick function here

    bIsRegistered = true;
  }
  else
  {
    // TODO Add assert to avoid multi registeration
  }
}

void FARPhysicsTickFunctionInterface::UnregisterPhysicsTickFunction()
{
  if (IsTickFunctionRegistered())
  {
    bIsRegistered = false;
  }
}

void FARPhysicsTickFunctionInterface::SetEnable(bool bEnabled)
{

}

void FARPhysicsMagneticTickFunction::ExecuteTick(const FARPhysicsTickParameters& Params)
{

}
