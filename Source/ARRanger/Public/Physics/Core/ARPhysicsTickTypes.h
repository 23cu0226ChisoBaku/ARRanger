/**
 * @file ARPhysicsTickTypes.h
 * @author MAI ZHICONG
 * @brief Definitions of specific TickFunction used by ARPhysicsTickObject
 */

#pragma once

#ifndef _AR_CORE_PHYSICS_TICK_TYPES_
#define _AR_CORE_PHYSICS_TICK_TYPES_

#include "Physics/Core/ARPhysicsTypes.h"

/**前方宣言 */
struct FARPhysicsTickParameters;
namespace ARRanger::Physics
{
  class FARPhysicsTickTask;
}

#define ARPHYSICS_API ARRANGER_API

/**
 * @brief AR物理Tick専用パラメータ構造体
 */
struct FARPhysicsTickParameters
{
  float DeltaTime;
};

enum class EARPhysicsTickType : uint8
{
  TT_Magnetic,
  TT_Gravity,
};

/**
 * @brief TickFunctionの基底クラス
 */
class FARPhysicsTickFunctionInterface
{
  friend class FARPhysicsTickManager;
  friend class ARRanger::Physics::FARPhysicsTickTask;

  public:

    EARPhysicsTickType PhysicsTickType;

    EPhysicsExecuteFrequency Frequency;

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
    ARPHYSICS_API FARPhysicsTickFunctionInterface();
    ARPHYSICS_API virtual ~FARPhysicsTickFunctionInterface();

    /**
     * @brief TickFunctionをTickManagerに登録する
     */
    ARPHYSICS_API void RegisterPhysicsTickFunction();

    /**
     * @brief TickFunctionを解読する
     */
    ARPHYSICS_API void UnregisterPhysicsTickFunction();

    /**
     * @brief TickFunctionの有効化を設定する
     */
    ARPHYSICS_API void SetEnable(bool bEnabled);

    bool IsEnabled() const { return m_internalData.IsValid() && m_internalData->TickState == Enabled; }
    bool IsTickFunctionRegistered() const { return m_internalData.IsValid() && m_internalData->bIsRegistered; }

    /**
     * @brief TickFunctionを実行する
     * @param Params Tickパラメータ構造体
     */
    ARPHYSICS_API virtual void ExecuteTick(const FARPhysicsTickParameters& Params) = 0;

  private:
    TUniquePtr<FInternalData> m_internalData;
};

/**
 * @brief 物理タスクTickFunction
 * 
 */
class FARPhysicsTickFunction : public FARPhysicsTickFunctionInterface
{

public:
  /**物理TickObject */
  class UARPhysicsTickObject* TargetObject;

protected:
  ARPHYSICS_API virtual void ExecuteTick(const FARPhysicsTickParameters& Params) override;
};

#undef ARPHYSICS_API

#endif // _AR_CORE_PHYSICS_TICK_TYPES_