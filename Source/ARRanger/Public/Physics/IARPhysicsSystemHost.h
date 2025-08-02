// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "IARPhysicsSystemHost.generated.h"

class IARMagnetizableInterface;

namespace ARRanger::Private
{
  struct FARPhysicsInitializer
  {
    ARRANGER_API static void InitializeARPhysicsInWorld(UWorld* World);
    ARRANGER_API static void InitializeARPhysicsInWorldWithActorType(UWorld* World, TSubclassOf<class AARPhysicsTickProcessorActor> Subclass = nullptr);

    private:
      FARPhysicsInitializer() = delete;
  };
}

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UARPhysicsSystemHost : public UInterface
{
	GENERATED_BODY()
};


class FARPhysicsEngine;

/**
 * 
 */
class ARRANGER_API IARPhysicsSystemHost
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
  void Physics_RequestMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);
};
