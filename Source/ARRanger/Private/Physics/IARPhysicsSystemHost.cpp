// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/IARPhysicsSystemHost.h"

#include "IARMagnetizableInterface.h"
#include "Physics/Core/ARPhysicsEngine.h"
#include "Physics/Core/ARPhysicsTickProcessorActor.h"
#include "Physics/Gameplay/ARPhysicsGlobal.h"

// Log Header
#include "Internal/ARLoggingHeader.h"

namespace ARRanger::Private
{
  /**
   * @brief AR物理エンジンを取得
   */
  inline static FARPhysicsEngine& GetEngine()
  {
    static FARPhysicsEngine Engine;
    return Engine;
  }

  void FARPhysicsCore::InitializeARPhysicsInWorld(UWorld* World)
  {
    check(World != nullptr);
    InitializeARPhysicsInWorldWithActorType(World, AARPhysicsTickProcessorActor::StaticClass());
  }

  void FARPhysicsCore::InitializeARPhysicsInWorldWithActorType(UWorld* World, TSubclassOf<class AARPhysicsTickProcessorActor> Subclass)
  {
    check(World != nullptr);
    check(Subclass != nullptr);

    FARPhysicsEngineInitializationParameters param;
    param.World = World;
    param.SubclassOfPTPActor = Subclass;

    GetEngine().InitializePhysicsEngine(param);
  }

  void FARPhysicsCore::DeinitializeARPhysics()
  {
    GetEngine().DeinitializePhysicsEngine();
  }
} // namespace ARRanger::Private

using ARRanger::Private::GetEngine;

void IARPhysicsSystemHost::Physics_RegisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  Physics_RequestMagneticTaskImpl(InSource, InTarget, EMagneticTaskFrequency::Constantly);
}

void IARPhysicsSystemHost::Physics_RegisterMagneticTask_Once(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  Physics_RequestMagneticTaskImpl(InSource, InTarget, EMagneticTaskFrequency::Once);
}

void IARPhysicsSystemHost::Physics_UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  FARPhysicsUnregistry termination;
  termination.Source = InSource;
  termination.Target = InTarget;
  termination.Type = EPhysicsUnregistryType::UnregisterMagnetic;

  GetEngine().UnregisterPhysicsProcess(termination);
}

void IARPhysicsSystemHost::Physics_RequestMagneticTaskImpl(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EMagneticTaskFrequency Frequency)
{
  if ((InSource == nullptr) || (InTarget == nullptr))
  {
    AR_LOG(LogARPhysics, Error, TEXT("Input is Invalid. Caller:[%s]"), *GetNameSafe(InSource->GetActor()));
    return;
  }

  FARPhysicsRegistry request;
  request.Source = InSource;
  request.Target = InTarget;
  
  using enum EARMagnetismType;
  if ((InSource->GetMagnetismType() == Attraction) && (InTarget->GetMagnetismType() == Attraction))
  {
    request.Type = EPhysicsRegistryType::RequestAttraction;
  }
  else if ((InSource->GetMagnetismType() == Repulsion) && (InTarget->GetMagnetismType() == Repulsion))
  {
    request.Type = EPhysicsRegistryType::RequestRepulsion;
  }
  else
  {
    AR_LOG(LogARPhysics, Warning, TEXT("Request type is NONE."));
    request.Type = EPhysicsRegistryType::None;
    return;
  }

  // TODO 二種類のEnumを利用する手間を減らしたい
  switch (Frequency)
  {
    case Once:
    {
      request.Frequency = EPhysicsExecuteFrequency::Once;
    }
    break;
    case Constantly:
    {
      request.Frequency = EPhysicsExecuteFrequency::Constantly;
    }
    break;
  }

  GetEngine().RegisterPhysicsTask(request);
}

