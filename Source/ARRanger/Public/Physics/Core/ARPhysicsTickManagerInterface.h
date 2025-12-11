/**
 * @file ARPhysicsTickManagerInterface.h
 * @author your name (you@domain.com)
 * @brief 物理制御TickManagerのインターフェイス
 */
#pragma once

#ifndef _AR_CORE_PHYSICS_TICK_MANAGER_INTERFACE_
#define _AR_CORE_PHYSICS_TICK_MANAGER_INTERFACE_

#include "Physics/Core/ARPhysicsTickTypes.h"

/**前方宣言 */
struct FARPhysicsTickParameters;

namespace ARRanger::Physics
{
  /**
   * @brief Interface of ARPhysics tick manager
   */
  class IARPhysicsTickManagerInterface
  {
    public:
      /**
       * @brief TickManagerインターフェイスを取得（シングルトン）
       * @return IARPhysicsTickManagerInterface& 
       */
      static ARRANGER_API IARPhysicsTickManagerInterface& Get();

      ARRANGER_API virtual ~IARPhysicsTickManagerInterface() = default;
    
      /**
       * @brief 特定TickタイプのTickタスクを実行する
       * @param TickType 
       * @param Params Tickパラメータ構造体
       */
      ARRANGER_API virtual void ExecuteARPhysicsTick(EARPhysicsTickType TickType, const FARPhysicsTickParameters& Params) = 0;

      /**
       * @brief Tickタスクを全て解読する
       * 全てのTickタスクが止まる
       */
      ARRANGER_API virtual void DeinitializeTickTasks() = 0; 
  };
} // namespace ARRanger::Physics


#endif // _AR_CORE_PHYSICS_TICK_MANAGER_INTERFACE_
