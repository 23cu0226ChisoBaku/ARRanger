// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class IARMagnetizableInterface;
class FARPhysicsEngineProxy;
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
    ARRANGER_API ~FARPhysicsEngine();
    
    ARRANGER_API void InitializePhysicsEngine(const FARPhysicsEngineInitializationParameters& Parameters);
    ARRANGER_API void DeinitializePhysicsEngine();
    ARRANGER_API void RequestPhysicsProcess(const FARPhysicsRequest& Request);
    ARRANGER_API PhysicsEngineProxyPtr GetProxy() const { return m_proxy.Get(); }

  private:
  AARPhysicsTickProcessorActor* InitializePhysicsTickProcessorActor(UWorld* World, TSubclassOf<AARPhysicsTickProcessorActor> Subclass);
    TSharedPtr<FARPhysicsEngineProxy> MakePhysicsEngineProxy() const;

  private:

    TSharedPtr<FARPhysicsEngineProxy> m_proxy;
    TWeakObjectPtr<AARPhysicsTickProcessorActor> m_tickProcessorActor;
};
