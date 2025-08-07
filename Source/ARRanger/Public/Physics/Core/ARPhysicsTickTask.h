#pragma once

#ifndef _AR_CORE_PHYSICS_TICK_TASK_
#define _AR_CORE_PHYSICS_TICK_TASK_

struct FARPhysicsTickParameters;
class FARPhysicsTickFunctionInterface;

namespace ARRanger
{

namespace Physics
{
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