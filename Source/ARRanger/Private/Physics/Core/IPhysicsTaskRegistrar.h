/**
 * @file IPhysicsTaskRegistrar.h
 * @author MAI ZHICONG
 * @brief 物理タスク登録者インターフェイス
 * 
 */

#pragma once

#include "UObject/Interface.h"

#include "IPhysicsTaskRegistrar.generated.h"

class IARMagnetizableInterface;
enum class EPhysicsExecuteFrequency : uint8;
enum class EPhysicsRegistryType : uint8;

#define ARPHYSICS_API ARRANGER_API

UINTERFACE(MinimalAPI)
class UPhysicsTaskRegistrar : public UInterface
{
	GENERATED_BODY()
};

class IPhysicsTaskRegistrar
{
	GENERATED_BODY()

public:
  ARPHYSICS_API virtual void RegisterMagneticTask(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj, EPhysicsRegistryType InRequestType, EPhysicsExecuteFrequency InFrequency) = 0;
  ARPHYSICS_API virtual void UnregisterMagneticTask(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj) = 0;
};
