// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "IPhysicsTaskRegistrar.generated.h"

class IARMagnetizableInterface;
enum class EPhysicsExecuteFrequency : uint8;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPhysicsTaskRegistrar : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IPhysicsTaskRegistrar
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
  ARRANGER_API virtual void RegisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsRegistryType InRequestType, EPhysicsExecuteFrequency InFrequency) = 0;
  ARRANGER_API virtual void UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget) = 0;
};
