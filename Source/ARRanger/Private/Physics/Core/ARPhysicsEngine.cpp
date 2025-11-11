// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/Core/ARPhysicsEngine.h"

#include "Magnetic/IARMagnetizableInterface.h"
#include "Internal/ARLoggingHeader.h"
#include "Physics/Core/ARPhysicsEngineProxy.h"
#include "Physics/Core/ARPhysicsTickProcessorActor.h"
#include "Physics/Core/IPhysicsTaskRegistrar.h"


/**インスタンス数制限クラス定義 */
DEFINE_COUNT_LIMITER_PROPERTY(FARPhysicsEngine)

FARPhysicsEngine::FARPhysicsEngine()
  : m_proxy{nullptr}
  , m_taskRegistrar{nullptr}
{ }

FARPhysicsEngine::~FARPhysicsEngine()
{
  DeinitializePhysicsEngine();
}

void FARPhysicsEngine::InitializePhysicsEngine(const FARPhysicsEngineInitializationParameters& Parameters)
{
  m_proxy = MakePhysicsEngineProxy();
  if (m_proxy.IsValid())
  {
    m_proxy->Initialize(this);
  }

  if (Parameters.TaskRegistrar != nullptr)
  {
    m_taskRegistrar = Parameters.TaskRegistrar;
  }
}

void FARPhysicsEngine::DeinitializePhysicsEngine()
{
  m_proxy.Reset();
  m_taskRegistrar.Reset();
}

void FARPhysicsEngine::RegisterPhysicsTask(const FARPhysicsRegistry& Registry)
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

  if (GetProxy() == nullptr)
  {
    AR_LOG(LogARPhysics, Error, TEXT("Initialize AR physics engine FIRST!"));
    return;
  }

  // 磁力タスク登録
  if (Registry.IsMagneticForceType())
  {
    // m_tickProcessorActor->RegisterMagneticTask(Registry.Source, Registry.Target, Registry.Type, Registry.Frequency);
    m_taskRegistrar->RegisterMagneticTask(Registry.Source, Registry.Target, Registry.Type, Registry.Frequency);
  }
}

void FARPhysicsEngine::UnregisterPhysicsProcess(const FARPhysicsUnregistry& Unregistry)
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

  if (GetProxy() == nullptr)
  {
    AR_LOG(LogARPhysics, Error, TEXT("Initialize AR physics engine FIRST!"));
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

TSharedPtr<FARPhysicsEngineProxy> FARPhysicsEngine::MakePhysicsEngineProxy() const
{
  return ::MakeShared<FARPhysicsEngineProxy>();
}

