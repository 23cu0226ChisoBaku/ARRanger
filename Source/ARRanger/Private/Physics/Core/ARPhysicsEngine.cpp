// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Core/ARPhysicsEngine.h"

#include "Physics/Core/ARPhysicsEngineProxy.h"
#include "Physics/Core/ARPhysicsTickProcessorActor.h"

#include "IARMagnetizableInterface.h"

#include "Internal/ARLoggingHeader.h"

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

void FARPhysicsEngine::RequestPhysicsProcess(const FARPhysicsRequest& Request)
{
  // FIXME Implement immediately
  if ((Request.Source == nullptr) || (Request.Target == nullptr))
  {
    AR_LOG(LogARPhysics, Warning, TEXT("Invalid request"));
    return;
  }

  PhysicsEngineProxyPtr proxyPtr = GetProxy();
  if (proxyPtr == nullptr)
  {
    AR_LOG(LogARPhysics, Error, TEXT("Initialize AR physics engine FIRST!"));
    return;
  }

  if (Request.IsMagneticForceType())
  {
    using enum EPhysicsRequestFrequency;
    switch (Request.Frequency)
    {
      case Once:
      {
        FARPhysicsSimulationParam simulationParam(*Request.Source, *Request.Target);
        using enum EPhysicsRequestType;
        switch (Request.Type)
        {
          case RequestAttraction:
          {
            proxyPtr->SimulateAttraction(simulationParam);
          }
          break;
          case RequestRepulsion:
          {
            proxyPtr->SimulateRepulsion(simulationParam);
          }
          break;
        }
      }
      case Constantly:
      {
        using enum EPhysicsRequestType;
        switch (Request.Type)
        {
          case RequestAttraction:
          {
            m_tickProcessorActor->RegisterMagneticTask(Request.Source, Request.Target, EPhysicsTickType::TickAttraction);
          }
          break;
          case RequestRepulsion:
          {
            m_tickProcessorActor->RegisterMagneticTask(Request.Source, Request.Target, EPhysicsTickType::TickRepulsion);
          }
          break;
        }
      }
    }
  }
}

void FARPhysicsEngine::TerminatePhysicsProcess(const FARPhysicsTermination& Termination)
{
  PhysicsEngineProxyPtr proxyPtr = GetProxy();
  if (proxyPtr == nullptr)
  {
    AR_LOG(LogARPhysics, Error, TEXT("Initialize AR physics engine FIRST!"));
    return;
  }

  using enum EPhysicsTerminationType;
  switch (Termination.Type)
  {
    case TerminateMagnetic:
    {
      m_tickProcessorActor->UnregisterMagneticTask(Termination.Source, Termination.Target);
    }
    break;
  }
}

TSharedPtr<FARPhysicsEngineProxy> FARPhysicsEngine::MakePhysicsEngineProxy() const
{
  return ::MakeShared<FARPhysicsEngineProxy>();
}

void FARPhysicsEngine::InitializePhysicsTickProcessorActor(UWorld* World, TSubclassOf<AARPhysicsTickProcessorActor> Subclass)
{
  if ((World == nullptr) || (Subclass == nullptr))
  {
    return;
  }

  AARPhysicsTickProcessorActor* spawnedActor = World->SpawnActorDeferred<AARPhysicsTickProcessorActor>(Subclass, FTransform::Identity);
  check(spawnedActor != nullptr);

  spawnedActor->OnSpawnActor(this);
  spawnedActor->FinishSpawning(FTransform::Identity);

  m_tickProcessorActor = spawnedActor;
}
