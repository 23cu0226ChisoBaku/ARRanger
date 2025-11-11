/**
 * @file ARPhysicsTickManagerInterface.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#ifndef _AR_CORE_PHYSICS_TICK_MANAGER_INTERFACE_
#define _AR_CORE_PHYSICS_TICK_MANAGER_INTERFACE_

#include "Physics/Core/ARPhysicsTickTypes.h"

struct FARPhysicsTickParameters;

namespace ARRanger::Physics
{
  /**
   * @brief Interface of ARPhysics tick manager
   */
  class IARPhysicsTickManagerInterface
  {
    public:
      static ARRANGER_API IARPhysicsTickManagerInterface& Get();

      ARRANGER_API virtual ~IARPhysicsTickManagerInterface() = default;
    
      /**
       * @brief Execute tick function by tick type
       * @param TickType 
       * @param Params Parameters for physics tick
       */
      ARRANGER_API virtual void ExecuteARPhysicsTick(EARPhysicsTickType TickType, const FARPhysicsTickParameters& Params) = 0;

      /**
       * @brief Deinitialize all tick tasks
       * This will stop all ARRanger physics tasks
       */
      ARRANGER_API virtual void DeinitializeTickTasks() = 0; 
  };
}


#endif // _AR_CORE_PHYSICS_TICK_MANAGER_INTERFACE_
