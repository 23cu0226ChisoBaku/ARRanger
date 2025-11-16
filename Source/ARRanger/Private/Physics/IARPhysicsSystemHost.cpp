// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/IARPhysicsSystemHost.h"

#include "Magnetic/IARMagnetizableInterface.h"
#include "Physics/Core/ARPhysicsEngine.h"
#include "Physics/Core/ARPhysicsTickProcessorActor.h"
#include "Physics/Gameplay/ARPhysicsGlobal.h"
#include "Physics/Core/ARPhysicsTypes.h"
#include "Physics/Core/IPhysicsTaskRegistrar.h"

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

  void FARPhysicsCore::InitializeARPhysics(IPhysicsTaskRegistrar* InTaskRegistrar)
  {
    check(InTaskRegistrar != nullptr);
    FARPhysicsEngineInitializationParameters param{};
    param.TaskRegistrar = InTaskRegistrar;
    
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
  Physics_RequestMagneticTaskImpl(InSource, InTarget, EPhysicsExecuteFrequency::Constantly);
}

void IARPhysicsSystemHost::Physics_RegisterMagneticTask_Once(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  Physics_RequestMagneticTaskImpl(InSource, InTarget, EPhysicsExecuteFrequency::Once);
}

void IARPhysicsSystemHost::Physics_UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  FARPhysicsUnregistry termination{};
  termination.Source = InSource;
  termination.Target = InTarget;
  termination.Type = EPhysicsUnregistryType::UnregisterMagnetic;

  GetEngine().UnregisterPhysicsProcess(termination);
}

void IARPhysicsSystemHost::Physics_RequestMagneticTaskImpl(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsExecuteFrequency Frequency)
{
  if ((InSource == nullptr) || (InTarget == nullptr))
  {
    AR_LOG(LogARPhysics, Error, TEXT("Input is Invalid. Caller:[%s]"), *GetNameSafe(InSource->GetActor()));
    return;
  }

  FARPhysicsRegistry request{};
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

  request.Frequency = Frequency;

  GetEngine().RegisterPhysicsTask(request);
}

