// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Core/ARPhysicsEngine.h"

#include "Physics/Core/ARPhysicsEngineProxy.h"
#include "Physics/Core/ARPhysicsTickProcessorActor.h"

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
    m_proxy->Initialize(AsShared());
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

void FARPhysicsEngine::RequestPhysicsProcess(const FARPhysicsRequest& Request)
{
  // FIXME Implement immediately
  if ((Request.Source == nullptr) || (Request.Target == nullptr))
  {
    return;
  }

  PhysicsEngineProxyPtr proxyPtr = GetProxy();
  if (proxyPtr != nullptr)
  {
  }
}

TSharedPtr<FARPhysicsEngineProxy> FARPhysicsEngine::MakePhysicsEngineProxy() const
{
  return ::MakeShared<FARPhysicsEngineProxy>();
}

AARPhysicsTickProcessorActor* FARPhysicsEngine::InitializePhysicsTickProcessorActor(UWorld* World, TSubclassOf<AARPhysicsTickProcessorActor> Subclass)
{
  if ((World == nullptr) || (Subclass == nullptr))
  {
    return nullptr;
  }

  AARPhysicsTickProcessorActor* spawnedActor = World->SpawnActorDeferred<AARPhysicsTickProcessorActor>(Subclass, FTransform::Identity);
  check(spawnedActor != nullptr);

  spawnedActor->OnSpawnActor(const_cast<FARPhysicsEngine*>(this)->AsShared());
  spawnedActor->FinishSpawning(FTransform::Identity);

  m_tickProcessorActor = spawnedActor;
}
