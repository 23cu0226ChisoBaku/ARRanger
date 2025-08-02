// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/IARPhysicsSystemHost.h"

#include "Physics/Core/ARPhysicsEngine.h"
#include "Physics/Core/ARPhysicsTickProcessorActor.h"

namespace ARRanger::Private
{
  struct FARPhysicsEngineAccessor
  {
    static FARPhysicsEngine& GetARPhysicEngine()
    {
      static FARPhysicsEngine Engine;
      return Engine;
    }

    private:
      FARPhysicsEngineAccessor() = delete;
      ~FARPhysicsEngineAccessor() = delete;
  };

  void FARPhysicsInitializer::InitializeARPhysicsInWorld(UWorld* World)
  {
    check(World != nullptr);

    InitializeARPhysicsInWorldWithActorType(World, AARPhysicsTickProcessorActor::StaticClass());
  }

  void FARPhysicsInitializer::InitializeARPhysicsInWorldWithActorType(UWorld* World, TSubclassOf<class AARPhysicsTickProcessorActor> Subclass = nullptr)
  {
    check(World != nullptr);
    check(Subclass != nullptr);

    FARPhysicsEngineInitializationParameters param;
    param.World = World;
    param.SubclassOfPTPActor = Subclass;

    FARPhysicsEngineAccessor::GetARPhysicEngine().InitializePhysicsEngine(param);
  }
} 


void IARPhysicsSystemHost::Physics_RequestMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  if ((InSource == nullptr) || (InTarget == nullptr))
  {
    return;
  }

  FARPhysicsRequest request;
  request.Source = InSource;
  request.Target = InTarget;

  ARRanger::Private::FARPhysicsEngineAccessor::GetARPhysicEngine().RequestPhysicsProcess(request);
}

