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

enum class EPhysicsRequestType
{
  None,
  RequestAttraction,
  RequestRepulsion,
};

enum class EPhysicsTerminationType
{
  None,
  TerminateMagnetic,
};

enum class EPhysicsRequestFrequency
{
  Never,
  Once,
  Constantly,
};

struct FARPhysicsRequest
{
  IARMagnetizableInterface* Source = nullptr;

  IARMagnetizableInterface* Target = nullptr;

  EPhysicsRequestType Type = EPhysicsRequestType::None;

  EPhysicsRequestFrequency Frequency = EPhysicsRequestFrequency::Never;

  __forceinline bool IsMagneticForceType() const
  {
    using enum EPhysicsRequestType;
    return Type == RequestAttraction || Type == RequestRepulsion;
  }
};



struct FARPhysicsTermination
{
  IARMagnetizableInterface* Source = nullptr;

  IARMagnetizableInterface* Target = nullptr;

  EPhysicsTerminationType Type = EPhysicsTerminationType::None;
};

struct FARPhysicsEngineInitializationParameters
{
  TObjectPtr<UWorld> World;

  TSubclassOf<AARPhysicsTickProcessorActor> SubclassOfPTPActor;
};

namespace ARRanger::Private
{
  template<typename UserType, uint8 MaxSize>
  class FCountLimiter
  {
    public:
      FCountLimiter();
      ~FCountLimiter();
      static uint8 GetMaxSize();
      static uint8 GetCreatedObjectNum();
  };
}

#define DECLARE_COUNT_LIMITER_PROPERTY(UserType, MaxSize) \
  private: \
    friend class ARRanger::Private::FCountLimiter<UserType, MaxSize>; \
    static uint8 NumInstance; \
  public: \
    using ARRanger::Private::FCountLimiter<UserType, MaxSize>::GetMaxSize; \
    using ARRanger::Private::FCountLimiter<UserType, MaxSize>::GetCreatedObjectNum; \
  private: 

#define DEFINE_COUNT_LIMITER_PROPERTY(UserType) \
  uint8 UserType::NumInstance = 0;

class FARPhysicsEngine : private ARRanger::Private::FCountLimiter<FARPhysicsEngine, 1>
{
  using PhysicsEngineProxyPtr = FARPhysicsEngineProxy*;

  // Define this for FCountLimiter
  DECLARE_COUNT_LIMITER_PROPERTY(FARPhysicsEngine, 1)

  public:
    ARRANGER_API FARPhysicsEngine();
    ARRANGER_API virtual ~FARPhysicsEngine();

    ARRANGER_API void InitializePhysicsEngine(const FARPhysicsEngineInitializationParameters& Parameters);
    ARRANGER_API void DeinitializePhysicsEngine();
    ARRANGER_API void RequestPhysicsProcess(const FARPhysicsRequest& Request);
    ARRANGER_API void TerminatePhysicsProcess(const FARPhysicsTermination& Termination);
    ARRANGER_API PhysicsEngineProxyPtr GetProxy() const { return m_proxy.Get(); }

    AARPhysicsTickProcessorActor* GetPhysicsTickProcessor_Internal() const { return m_tickProcessorActor.Get(); }

  protected:
    ARRANGER_API virtual TSharedPtr<FARPhysicsEngineProxy> MakePhysicsEngineProxy() const;

  private:
    void InitializePhysicsTickProcessorActor(UWorld* World, TSubclassOf<AARPhysicsTickProcessorActor> Subclass);

  private:

    TSharedPtr<FARPhysicsEngineProxy> m_proxy;
    TWeakObjectPtr<AARPhysicsTickProcessorActor> m_tickProcessorActor;
};

// Include FCountLimiter inline file
#include "Internal/CountLimiterImpl.inl"

#endif // _AR_PHYSICS_ENGINE_
