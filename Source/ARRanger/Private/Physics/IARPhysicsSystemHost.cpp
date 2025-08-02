// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/IARPhysicsSystemHost.h"

#include "IARMagnetizableInterface.h"
#include "Physics/Core/ARPhysicsEngine.h"
#include "Physics/Core/ARPhysicsTickProcessorActor.h"

#include "ARLoggingHeader.h"

namespace ARRanger::Private
{
  struct FARPhysicsEngineAccessor
  {
    static FARPhysicsEngine& GetEngine()
    {
      static FARPhysicsEngine Engine;
      return Engine;
    }

    private:
      FARPhysicsEngineAccessor() = delete;
      ~FARPhysicsEngineAccessor() = delete;
  };

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

    FARPhysicsEngineAccessor::GetEngine().InitializePhysicsEngine(param);
  }

  void FARPhysicsCore::DeinitializeARPhysics()
  {
    FARPhysicsEngineAccessor::GetEngine().DeinitializePhysicsEngine();
  }
} 

using ARRanger::Private::FARPhysicsEngineAccessor;

void IARPhysicsSystemHost::Physics_RequestMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  if ((InSource == nullptr) || (InTarget == nullptr))
  {
    AR_LOG(LogARPhysics, Error, TEXT("Input is Invalid. Caller:[%s]"), *GetNameSafe(InSource->GetActor()));
    return;
  }

  FARPhysicsRequest request;
  request.Source = InSource;
  request.Target = InTarget;
  
  using enum EARMagnetismType;
  if ((InSource->GetMagnetismType() == Attraction) && (InTarget->GetMagnetismType() == Attraction))
  {
    request.Type = EPhysicsRequestType::RequestAttraction;
  }
  else if ((InSource->GetMagnetismType() == Repulsion) && (InTarget->GetMagnetismType() == Repulsion))
  {
    request.Type = EPhysicsRequestType::RequestRepulsion;
  }
  else
  {
    request.Type = EPhysicsRequestType::None;
  }

  FARPhysicsEngineAccessor::GetEngine().RequestPhysicsProcess(request);
}

