#include "Physics/Core/ARPhysicsTickTask.h"
#include "Physics/Core/ARPhysicsTickTypes.h"

namespace ARRanger
{

namespace Physics
{

FARPhysicsTickTask::FARPhysicsTickTask()
  : m_enabledTickFunctions{}
  , m_disabledTickFunctions{}
{ }

FARPhysicsTickTask::~FARPhysicsTickTask()
{
  for (const auto& tickFunc : m_enabledTickFunctions)
  {
    tickFunc->m_internalData->bIsRegistered = false;
  }

  for (const auto& tickFunc : m_disabledTickFunctions)
  {
    tickFunc->m_internalData->bIsRegistered = false;
  }
}

void FARPhysicsTickTask::ExecuteTask(const FARPhysicsTickParameters& TickParams)
{
  for (const auto& tickFunc : m_enabledTickFunctions)
  {
    check(tickFunc != nullptr);
    tickFunc->ExecuteTick(TickParams);
  }
}

void FARPhysicsTickTask::AddTickFunction(FARPhysicsTickFunctionInterface* TickFunction)
{
  check(!HasTickFunction(TickFunction));
  if (TickFunction->m_internalData->TickState == FARPhysicsTickFunctionInterface::ETickState_Internal::Enabled)
  {
    m_enabledTickFunctions.Emplace(TickFunction);
  }
  else if (TickFunction->m_internalData->TickState == FARPhysicsTickFunctionInterface::ETickState_Internal::Disabled)
  {
    m_disabledTickFunctions.Emplace(TickFunction);
  }
}

void FARPhysicsTickTask::RemoveTickFunction(FARPhysicsTickFunctionInterface* TickFunction);
{
  check(TickFunction != nullptr);
  switch (TickFunction->m_internalData->TickState)
  {
    case FARPhysicsTickFunctionInterface::ETickState_Internal::Enabled:
    {
      check(m_enabledTickFunctions.Remove(TickFunction) == 1);
    }
    break;

    case FARPhysicsTickFunctionInterface::ETickState_Internal::Disabled:
    {
      check(m_disabledTickFunctions.Remove(TickFunction) == 1);
    }
  }
}

bool FARPhysicsTickTask::HasTickFunction(const FARPhysicsTickFunctionInterface* TickFunction)
{
  #error Start here
  return true;
}
} // namespace ARRanger::Physics

} // namespace ARRanger