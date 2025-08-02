// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef _AR_PHYSICS_ENGINE_
#define _AR_PHYSICS_ENGINE_

/**前方宣言 */
class IARMagnetizableInterface;
class FARPhysicsEngineProxy;
class AARPhysicsTickProcessorActor;
class UWorld;
class AARPhysicsTickProcessorActor;

struct FARPhysicsRequest
{
  IARMagnetizableInterface* Source = nullptr;
  IARMagnetizableInterface* Target = nullptr;
};

struct FARPhysicsEngineInitializationParameters
{
  TObjectPtr<UWorld> World;

  TSubclassOf<AARPhysicsTickProcessorActor> SubclassOfPTPActor;
};

class FARPhysicsEngine : public TSharedFromThis<FARPhysicsEngine>
{
  using PhysicsEngineProxyPtr = FARPhysicsEngineProxy*;

  public:
    ARRANGER_API FARPhysicsEngine();
    ARRANGER_API virtual ~FARPhysicsEngine();

    ARRANGER_API void InitializePhysicsEngine(const FARPhysicsEngineInitializationParameters& Parameters);
    ARRANGER_API void DeinitializePhysicsEngine();
    ARRANGER_API void RequestPhysicsProcess(const FARPhysicsRequest& Request);
    ARRANGER_API PhysicsEngineProxyPtr GetProxy() const { return m_proxy.Get(); }

  protected:
    ARRANGER_API virtual TSharedPtr<FARPhysicsEngineProxy> MakePhysicsEngineProxy() const;
  private:
    void InitializePhysicsTickProcessorActor(UWorld* World, TSubclassOf<AARPhysicsTickProcessorActor> Subclass);
  private:

    TSharedPtr<FARPhysicsEngineProxy> m_proxy;
    TWeakObjectPtr<AARPhysicsTickProcessorActor> m_tickProcessorActor;
};

#endif // _AR_PHYSICS_ENGINE_
