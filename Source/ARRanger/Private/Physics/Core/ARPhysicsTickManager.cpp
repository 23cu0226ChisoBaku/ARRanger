#include "Physics/Core/ARPhysicsTickManagerInterface.h"
#include "Physics/Core/ARPhysicsTickTypes.h"
#include "Physics/Core/ARPhysicsTickTask.h"

using ARRanger::Physics::FARPhysicsTickTask;

class FARPhysicsTickManager : public ARRanger::Physics::FARPhysicsTickManagerInterface
{
  public:
    ~FARPhysicsTickManager()
    {
      DeinitializeTickTasks();
    }

    static FARPhysicsTickManager& Get()
    {
      static FARPhysicsTickManager s_instance;
      return s_instance;
    }  

    void AddARPhysicsTickFunction(FARPhysicsTickFunctionInterface* TickFunction)
    {
      check(TickFunction != nullptr)

      FARPhysicsTickTask* task = GetValidTickTask(TickFunction->PhysicsTickType);
      task->AddTickFunction(TickFunction);  
    }

    void RemoveARPhysicsTickFunction(FARPhysicsTickFunctionInterface* TickFunction)
    {
      check(TickFunction != nullptr)

      FARPhysicsTickTask* task = GetValidTickTask(TickFunction->PhysicsTickType);
      task->RemoveTickFunction(TickFunction); 
    }

    void ExecuteARPhysicsTick(EARPhysicsTickType TickType, const FARPhysicsTickParameters& Params) override
    {
      if (m_tickTasks.Contains(TickType))
      {
        m_tickTasks[TickType]->ExecuteTask(Params);
      }
    }

    FARPhysicsTickTask* GetValidTickTask(EARPhysicsTickType TickType)
    {
      if (!m_tickTasks.Contains(TickType))
      {
        m_tickTasks.Emplace(TickType, AllocateTickTasks());
      }

      return m_tickTasks[TickType];
    }

    FARPhysicsTickTask* AllocateTickTasks()
    {
      return new FARPhysicsTickTask{};
    }

    void DeinitializeTickTasks() override
    {
      for (const auto& [ _ , task] : m_tickTasks)
      {
        delete task;
      }

      m_tickTasks.Reset();
    }

    bool HasTickFunction(FARPhysicsTickFunctionInterface* TickFunction)
    {
      check(TickFunction != nullptr);

      FARPhysicsTickTask* task = GetValidTickTask(TickFunction->PhysicsTickType);
      return ((task != nullptr) && task->HasTickFunction(TickFunction));
    }

  private:
    TMap<EARPhysicsTickType, FARPhysicsTickTask*> m_tickTasks;
};

namespace ARRanger::Physics
{
  FARPhysicsTickManagerInterface& FARPhysicsTickManagerInterface::Get()
  {
    return FARPhysicsTickManager::Get();
  }
}


#pragma region FARPhysicsTickFunctionInterface implementation
FARPhysicsTickFunctionInterface::FARPhysicsTickFunctionInterface()
  : PhysicsTickType{EARPhysicsTickType::TT_Magnetic}
  , Frequency{EARPhysicsTickFrequency::TF_Default}
  , m_internalData{::MakeUnique<FInternalData>()}
{ }

FARPhysicsTickFunctionInterface::~FARPhysicsTickFunctionInterface()
{
  UnregisterPhysicsTickFunction();
  m_internalData.Reset();
}

void FARPhysicsTickFunctionInterface::RegisterPhysicsTickFunction()
{
  if (!IsTickFunctionRegistered())
  {
    FARPhysicsTickManager::Get().AddARPhysicsTickFunction(this);
    m_internalData->bIsRegistered = true;
  }
}

void FARPhysicsTickFunctionInterface::UnregisterPhysicsTickFunction()
{
  if (IsTickFunctionRegistered())
  {
    FARPhysicsTickManager::Get().RemoveARPhysicsTickFunction(this);
    m_internalData->bIsRegistered = false;
  }
}

void FARPhysicsTickFunctionInterface::SetEnable(bool bEnabled)
{
  if (IsTickFunctionRegistered())
  {
    if (bEnabled && (m_internalData->TickState == Disabled))
    {
      FARPhysicsTickTask* task = m_internalData->PrivateTickTask;
      check(task != nullptr);
      task->RemoveTickFunction(this);
      m_internalData->TickState = bEnabled ? Enabled : Disabled;
      task->AddTickFunction(this);
    }
  }
  else
  {
    m_internalData->TickState = bEnabled ? Enabled : Disabled;
  }
}

#pragma endregion FARPhysicsTickFunctionInterface implementation