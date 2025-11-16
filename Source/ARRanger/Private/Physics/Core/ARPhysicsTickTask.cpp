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

  m_enabledTickFunctions.Reset();
  m_disabledTickFunctions.Reset();
}

void FARPhysicsTickTask::ExecuteTask(const FARPhysicsTickParameters& TickParams)
{
  static TArray<FARPhysicsTickFunctionInterface*> s_removedTickFunctions{};

  for (const auto& tickFunc : m_enabledTickFunctions)
  {
    check(tickFunc != nullptr);
    tickFunc->ExecuteTick(TickParams);

    // 一回しか実行しないTickFunctionは実行後解読
    if (tickFunc->Frequency == EPhysicsExecuteFrequency::Once)
    {
      s_removedTickFunctions.Emplace(tickFunc);
    }
  }

  // 無効になったTickFunctionを解読
  for (const auto& tickFunc : s_removedTickFunctions)
  {
    tickFunc->m_internalData->bIsRegistered = false;
    RemoveTickFunction(tickFunc);
  }
  
  s_removedTickFunctions.Reset();
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

void FARPhysicsTickTask::RemoveTickFunction(FARPhysicsTickFunctionInterface* TickFunction)
{
  check(TickFunction != nullptr);

  using enum FARPhysicsTickFunctionInterface::ETickState_Internal;
  switch (TickFunction->m_internalData->TickState)
  {
    case Enabled:
    {
      const int32 removedNum = m_enabledTickFunctions.Remove(TickFunction);
      check(removedNum == 1);
    }
    break;

    case Disabled:
    {
      const int32 removedNum = m_enabledTickFunctions.Remove(TickFunction);
      check(removedNum == 1);
    }
    break;
  }
}

bool FARPhysicsTickTask::HasTickFunction(const FARPhysicsTickFunctionInterface* TickFunction)
{
  return m_enabledTickFunctions.Contains(TickFunction) || m_disabledTickFunctions.Contains(TickFunction);
}

} // namespace ARRanger::Physics

} // namespace ARRanger