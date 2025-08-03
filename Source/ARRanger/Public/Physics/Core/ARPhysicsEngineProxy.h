#pragma once

#ifndef _AR_PHYSICS_CORE_PHYSICS_ENGINE_PROXY_
#define _AR_PHYSICS_CORE_PHYSICS_ENGINE_PROXY_

class IARMagnetizableInterface;
class FARPhysicsEngine;

struct FARPhysicsSimulationParam
{
  FARPhysicsSimulationParam(IARMagnetizableInterface& InTarget1, IARMagnetizableInterface& InTarget2);

  IARMagnetizableInterface& Target1;
  IARMagnetizableInterface& Target2;
};

struct FARPhysicsCancellationParam
{
  const IARMagnetizableInterface* Target1 = nullptr;
  const IARMagnetizableInterface* Target2 = nullptr;
};

class FARPhysicsEngineProxy
{
  public:
    ARRANGER_API FARPhysicsEngineProxy();
    ARRANGER_API virtual ~FARPhysicsEngineProxy();

    ARRANGER_API void Initialize(FARPhysicsEngine* OwningPhysicsEnginePtr);
    ARRANGER_API void SimulateAttraction(const FARPhysicsSimulationParam& SimulationParameter);
    ARRANGER_API void SimulateRepulsion(const FARPhysicsSimulationParam& SimulationParameter);

  protected:
    ARRANGER_API virtual void OnInitialize(FARPhysicsEngine* OwningPhysicsEnginePtr);
    ARRANGER_API virtual void OnSimulateAttraction(const FARPhysicsSimulationParam& SimulationParameter);
    ARRANGER_API virtual void OnSimulateRepulsion(const FARPhysicsSimulationParam& SimulationParameter);

  private:
    FARPhysicsEngine* m_privatePhysicsEngine;
};

#endif // _AR_PHYSICS_CORE_PHYSICS_ENGINE_PROXY_