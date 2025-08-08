// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef _AR_CORE_PHYSICS_TICK_MANAGER_INTERFACE_
#define _AR_CORE_PHYSICS_TICK_MANAGER_INTERFACE_

#include "Physics/Core/ARPhysicsTickTypes.h"
struct FARPhysicsTickParameters;

namespace ARRanger::Physics
{
  class FARPhysicsTickManagerInterface
  {
    public:
      ARRANGER_API virtual ~FARPhysicsTickManagerInterface() = default;
    
      ARRANGER_API virtual void ExecuteARPhysicsTick(EARPhysicsTickType TickType, const FARPhysicsTickParameters& Params) = 0;

      ARRANGER_API virtual void DeinitializeTickTasks() = 0; 
    
      static ARRANGER_API FARPhysicsTickManagerInterface& Get();
  };
}


#endif // _AR_CORE_PHYSICS_TICK_MANAGER_INTERFACE_
