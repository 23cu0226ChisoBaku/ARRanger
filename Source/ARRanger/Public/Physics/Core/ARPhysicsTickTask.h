/**
 * @file ARPhysicsTickTask.h
 * @author MAI ZHICONG
 * @brief Tick task for ARPhysicsEngine
 */

#pragma once

#ifndef _AR_CORE_PHYSICS_TICK_TASK_
#define _AR_CORE_PHYSICS_TICK_TASK_

/**Forward declaration */
struct FARPhysicsTickParameters;
class FARPhysicsTickFunctionInterface;

namespace ARRanger
{

namespace Physics
{
  /**
   * @brief Base task for ARPhysicsTickManager
   * Handle groups of tick function interface
   * 
   */
  class FARPhysicsTickTask
  {
    public:
      ARRANGER_API FARPhysicsTickTask();
      ARRANGER_API ~FARPhysicsTickTask();
      ARRANGER_API void ExecuteTask(const FARPhysicsTickParameters& TickParams);
      ARRANGER_API void AddTickFunction(FARPhysicsTickFunctionInterface* TickFunction);
      ARRANGER_API void RemoveTickFunction(FARPhysicsTickFunctionInterface* TickFunction);
      ARRANGER_API bool HasTickFunction(const FARPhysicsTickFunctionInterface* TickFunction);

    private:
      TSet<FARPhysicsTickFunctionInterface*> m_enabledTickFunctions;
      TSet<FARPhysicsTickFunctionInterface*> m_disabledTickFunctions;
  };

} // namespace ARRanger::Physics

} // namespace ARRanger

#endif // _AR_CORE_PHYSICS_TICK_TASK_