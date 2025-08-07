// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "IARPhysicsSystemHost.generated.h"

class IARMagnetizableInterface;
class AARPhysicsTickProcessorActor;

namespace ARRanger::Private
{
  struct FARPhysicsCore
  {
    ARRANGER_API static void InitializeARPhysicsInWorld(UWorld* World);
    ARRANGER_API static void InitializeARPhysicsInWorldWithActorType(UWorld* World, TSubclassOf<AARPhysicsTickProcessorActor> Subclass = nullptr);
    ARRANGER_API static void DeinitializeARPhysics();

    private:
      FARPhysicsCore() = delete;
  };
}

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UARPhysicsSystemHost : public UInterface
{
	GENERATED_BODY()
};

class ARRANGER_API IARPhysicsSystemHost
{
	GENERATED_BODY()

  enum EMagneticTaskFrequency
  {
    Once,
    Constantly,
  };

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
  void Physics_RequestMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);
  void Physics_RequestMagneticTask_Once(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);
  void Physics_TerminateMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);

private:
  void Physics_RequestMagneticTaskImpl(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EMagneticTaskFrequency Frequency);
};
