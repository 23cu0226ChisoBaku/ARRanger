// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef _AR_CORE_PHYSICS_TICK_TYPES_
#define _AR_CORE_PHYSICS_TICK_TYPES_

struct FARPhysicsTickParameters;

enum class EARPhysicsTickType : uint8
{
  TT_Magnetic,
  TT_Gravity,
};

class FARPhysicsTickFunctionInterface
{
  private:
    enum ETickState_Internal
    { 
      Enabled,
      Disabled,
    };

  public:
    ARRANGER_API FARPhysicsTickFunctionInterface();
    ARRANGER_API virtual ~FARPhysicsTickFunctionInterface();

    ARRANGER_API void RegisterPhysicsTickFunction();
    ARRANGER_API void UnregisterPhysicsTickFunction();
    ARRANGER_API void SetEnable(bool bEnabled);
    bool IsEnabled() const { return m_tickState == ETickState_Internal::Enabled; }
    bool IsTickFunctionRegistered() const { return bIsRegistered; }

    ARRANGER_API virtual void ExecuteTick(const FARPhysicsTickParameters& Params) = 0;

  public:
    EARPhysicsTickType PhysicsTickType;

  private:
    ETickState_Internal m_tickState;
    uint8 bIsRegistered : 1;
};

class FARPhysicsMagneticTickFunction : public FARPhysicsTickFunctionInterface
{

public:
  class UARPhysicsTickObject* TargetObject;

protected:
  ARRANGER_API virtual void ExecuteTick(const FARPhysicsTickParameters& Params) override;

};

#endif // _AR_CORE_PHYSICS_TICK_TYPES_