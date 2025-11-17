#include "Physics/Core/ARPhysicsSystem.h"

#include "Magnetic/IARMagnetizableInterface.h"
#include "Internal/ARLoggingHeader.h"
#include "Physics/Core/ARPhysicsTickProcessorActor.h"
#include "Physics/Core/IPhysicsTaskRegistrar.h"

/**インスタンス数制限クラス定義 */
DEFINE_COUNT_LIMITER_PROPERTY(FARPhysicsSystem)

FARPhysicsSystem::FARPhysicsSystem()
  : m_taskRegistrar{nullptr}
{ }

FARPhysicsSystem::~FARPhysicsSystem()
{
  DeinitializePhysicsSystem();
}

void FARPhysicsSystem::InitializePhysicsSystem(const FARPhysicsSystemInitializationParameters& Parameters)
{
  if (Parameters.TaskRegistrar != nullptr)
  {
    m_taskRegistrar = Parameters.TaskRegistrar;
  }
}

void FARPhysicsSystem::DeinitializePhysicsSystem()
{
  m_taskRegistrar.Reset();
}

void FARPhysicsSystem::RegisterPhysicsTask(const FARPhysicsRegistry& Registry)
{
  if (!m_taskRegistrar.IsValid())
  {
    return;
  }
  
  if ((Registry.Source == nullptr) || (Registry.Target == nullptr))
  {
    AR_LOG(LogARPhysics, Warning, TEXT("Invalid registry"));
    return;
  }

  // 磁力タスク登録
  if (Registry.IsMagneticForceType())
  {
    m_taskRegistrar->RegisterMagneticTask(Registry.Source, Registry.Target, Registry.Type, Registry.Frequency);
  }
}

void FARPhysicsSystem::UnregisterPhysicsProcess(const FARPhysicsUnregistry& Unregistry)
{
  if (!m_taskRegistrar.IsValid())
  {
    return;
  }

  if ((Unregistry.Source == nullptr) || (Unregistry.Target == nullptr))
  {
    AR_LOG(LogARPhysics, Warning, TEXT("Invalid unregistry"));
    return;
  }

  // 登録解除命令をTickアクターに送る
  {
    using enum EPhysicsUnregistryType;
    switch (Unregistry.Type)
    {
      case UnregisterMagnetic:
      {
        m_taskRegistrar->UnregisterMagneticTask(Unregistry.Source, Unregistry.Target);
      }
      break;
    }
  }
}


