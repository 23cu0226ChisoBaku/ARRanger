// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/IARPhysicsSystemHost.h"

#include "Magnetic/IARMagnetizableInterface.h"
#include "Physics/Core/ARPhysicsSystem.h"
#include "Physics/Gameplay/ARPhysicsGlobal.h"
#include "Physics/Core/ARPhysicsTypes.h"
#include "Physics/Core/IPhysicsTaskRegistrar.h"

// Log Header
#include "Internal/ARLoggingHeader.h"

namespace ARRanger::Private
{
  /**
   * @brief AR物理システムを取得
   */
  inline static FARPhysicsSystem& GetPhysicsSystem()
  {
    static FARPhysicsSystem System;
    return System;
  }

  void FARPhysicsCore::InitializeARPhysics(IPhysicsTaskRegistrar* InTaskRegistrar)
  {
    check(InTaskRegistrar != nullptr);
    FARPhysicsSystemInitializationParameters param{};
    param.TaskRegistrar = InTaskRegistrar;
    
    GetPhysicsSystem().InitializePhysicsSystem(param);
  }

  void FARPhysicsCore::DeinitializeARPhysics()
  {
    GetPhysicsSystem().DeinitializePhysicsSystem();
  }
} // namespace ARRanger::Private

using ARRanger::Private::GetPhysicsSystem;

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

  GetPhysicsSystem().UnregisterPhysicsProcess(termination);
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
  // 同じタイプの磁力オブジェクトかつNoneじゃないタイプだとリクエストを拒否する
  if (InSource->GetMagnetismType() != InTarget->GetMagnetismType() || (InSource->GetMagnetismType() == None))
  {
    AR_LOG(LogARPhysics, Warning, TEXT("MagnetismType is not same or one of the type is None.Request denied."));
    return;
  }

  // 磁力リクエストタイプ設定
  if (InSource->GetMagnetismType() == Attraction)
  {
    request.Type = EPhysicsRegistryType::RequestAttraction;
  }
  else if (InSource->GetMagnetismType() == Repulsion)
  {
    request.Type = EPhysicsRegistryType::RequestRepulsion;
  }

  request.Frequency = Frequency;

  GetPhysicsSystem().RegisterPhysicsTask(request);
}

