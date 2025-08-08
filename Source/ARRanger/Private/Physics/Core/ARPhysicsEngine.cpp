// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/Core/ARPhysicsEngine.h"

#include "IARMagnetizableInterface.h"
#include "Internal/ARLoggingHeader.h"
#include "Physics/Core/ARPhysicsEngineProxy.h"
#include "Physics/Core/ARPhysicsTickProcessorActor.h"


/**インスタンス数制限クラス定義 */
DEFINE_COUNT_LIMITER_PROPERTY(FARPhysicsEngine)

FARPhysicsEngine::FARPhysicsEngine()
  : m_proxy{nullptr}
  , m_tickProcessorActor{nullptr}
{ }

FARPhysicsEngine::~FARPhysicsEngine()
{
  DeinitializePhysicsEngine();
}

void FARPhysicsEngine::InitializePhysicsEngine(const FARPhysicsEngineInitializationParameters& Parameters)
{
  if (!m_tickProcessorActor.IsValid())
  {
    InitializePhysicsTickProcessorActor(Parameters.World, Parameters.SubclassOfPTPActor); 
  }

  m_proxy = MakePhysicsEngineProxy();
  if (m_proxy.IsValid())
  {
    m_proxy->Initialize(this);
  }
}

void FARPhysicsEngine::DeinitializePhysicsEngine()
{
  if (m_tickProcessorActor.IsValid())
  {
    m_tickProcessorActor->Destroy();
    m_tickProcessorActor.Reset();
  }

  m_proxy.Reset();
}

void FARPhysicsEngine::RegisterPhysicsTask(const FARPhysicsRegistry& Registry)
{
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
    using enum EPhysicsExecuteFrequency;
    switch (Registry.Frequency)
    {
      case Once:
      {
        // FIXME Test purpose
        // FIXME Currently set tick function frequency in tick object class
        m_tickProcessorActor->RegisterMagneticTask(Registry.Source, Registry.Target, Registry.Type);
      }
      case Constantly:
      {
        m_tickProcessorActor->RegisterMagneticTask(Registry.Source, Registry.Target, Registry.Type);
      }
      break;
    }
  }
}

void FARPhysicsEngine::UnregisterPhysicsProcess(const FARPhysicsUnregistry& Unregistry)
{
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
        m_tickProcessorActor->UnregisterMagneticTask(Unregistry.Source, Unregistry.Target);
      }
      break;
    }
  }
}

TSharedPtr<FARPhysicsEngineProxy> FARPhysicsEngine::MakePhysicsEngineProxy() const
{
  return ::MakeShared<FARPhysicsEngineProxy>();
}

void FARPhysicsEngine::InitializePhysicsTickProcessorActor(UWorld* World, TSubclassOf<AARPhysicsTickProcessorActor> Subclass)
{
  check(World != nullptr);
  check(Subclass != nullptr);

  AARPhysicsTickProcessorActor* spawnedActor = World->SpawnActorDeferred<AARPhysicsTickProcessorActor>(Subclass, FTransform::Identity);
  check(spawnedActor != nullptr);

  // TickActor初期化
  spawnedActor->OnSpawnActor(this);
  spawnedActor->FinishSpawning(FTransform::Identity);

  m_tickProcessorActor = spawnedActor;
}
