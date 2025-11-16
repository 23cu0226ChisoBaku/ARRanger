/**
 * @file ARPhysicsTickTask.h
 * @author MAI ZHICONG
 * @brief Tick task for ARPhysicsEngine
 */

#pragma once

#ifndef _AR_CORE_PHYSICS_TICK_TASK_
#define _AR_CORE_PHYSICS_TICK_TASK_

/**前方宣言 */
struct FARPhysicsTickParameters;
class FARPhysicsTickFunctionInterface;

#define ARPHYSICS_API ARRANGER_API

namespace ARRanger
{

namespace Physics
{
  /**
   * @brief Base task for ARPhysicsTickManager
   * Handle groups of tick function interface
   * 
   */
  class FARPhysicsTickTask
  {
    public:
      ARPHYSICS_API FARPhysicsTickTask();
      ARPHYSICS_API ~FARPhysicsTickTask();

      /**
       * @brief タスクを実行する
       * @param TickParams Tickパラメータ構造体
       */
      ARPHYSICS_API void ExecuteTask(const FARPhysicsTickParameters& TickParams);

      /**
       * @brief タスクにTickFunctionを登録する
       * @param TickFunction 
       */
      ARPHYSICS_API void AddTickFunction(FARPhysicsTickFunctionInterface* TickFunction);

      /**
       * @brief タスクにTickFunctionを解読する
       * @param TickFunction 
       */
      ARPHYSICS_API void RemoveTickFunction(FARPhysicsTickFunctionInterface* TickFunction);

      /**
       * @brief 特定のTickFunctionがTaskに存在するかを確認する
       * @param TickFunction 
       * @return TickFunctionが有効かつ存在すると true、それ以外 false
       */
      ARPHYSICS_API bool HasTickFunction(const FARPhysicsTickFunctionInterface* TickFunction);

    private:
      TSet<FARPhysicsTickFunctionInterface*> m_enabledTickFunctions;
      TSet<FARPhysicsTickFunctionInterface*> m_disabledTickFunctions;
  };

} // namespace ARRanger::Physics

} // namespace ARRanger

#undef ARPHYSICS_API

#endif // _AR_CORE_PHYSICS_TICK_TASK_