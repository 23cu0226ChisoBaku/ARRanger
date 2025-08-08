// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef _AR_CORE_PHYSICS_TICK_TYPES_
#define _AR_CORE_PHYSICS_TICK_TYPES_

/**前方宣言 */
struct FARPhysicsTickParameters;
namespace ARRanger::Physics
{
  class FARPhysicsTickTask;
}

/**
 * @brief AR物理Tick専用パラメータ構造体
 */
struct FARPhysicsTickParameters
{
  /**
   * @brief DeltaTime
   */
  float DeltaTime;
  
  /**
   * @brief 合計シミュレーション時間
   */
  float TotalSimTime;
};

enum class EARPhysicsTickType : uint8
{
  TT_Magnetic,
  TT_Gravity,
};

enum class EARPhysicsTickFrequency : uint8
{
  /**Tick every time until tick function is disabled */
  TF_Default,

  /**Only tick once */
  TF_Once,
};

class FARPhysicsTickFunctionInterface
{
  friend class FARPhysicsTickManager;
  friend class ARRanger::Physics::FARPhysicsTickTask;

  public:
    EARPhysicsTickType PhysicsTickType;

    EARPhysicsTickFrequency Frequency;

  private:
    enum ETickState_Internal
    { 
      Enabled,
      Disabled,
    };

    struct FInternalData
    {
      ARRanger::Physics::FARPhysicsTickTask* PrivateTickTask = nullptr;

      ETickState_Internal TickState = Enabled;

      uint8 bIsRegistered : 1 = false;
    };

  public:
    ARRANGER_API FARPhysicsTickFunctionInterface();
    ARRANGER_API virtual ~FARPhysicsTickFunctionInterface();

    ARRANGER_API void RegisterPhysicsTickFunction();
    ARRANGER_API void UnregisterPhysicsTickFunction();
    ARRANGER_API void SetEnable(bool bEnabled);
    bool IsEnabled() const { return m_internalData.IsValid() && m_internalData->TickState == Enabled; }
    bool IsTickFunctionRegistered() const { return m_internalData.IsValid() && m_internalData->bIsRegistered; }

    ARRANGER_API virtual void ExecuteTick(const FARPhysicsTickParameters& Params) = 0;

  private:
    TUniquePtr<FInternalData> m_internalData;
};

class FARPhysicsTickFunction : public FARPhysicsTickFunctionInterface
{

public:
  class UARPhysicsTickObject* TargetObject;

protected:
  ARRANGER_API virtual void ExecuteTick(const FARPhysicsTickParameters& Params) override;

};

#endif // _AR_CORE_PHYSICS_TICK_TYPES_