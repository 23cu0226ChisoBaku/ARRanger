/**
 * @file IBattleSystemInterface.h
 * @brief バトルシステムインターフェイス
 */

#pragma once

#ifndef _ARRANGER_BATTLE_SYSTEM_INTERFACE_
#define _ARRANGER_BATTLE_SYSTEM_INTERFACE_

#define UE_API ARRANGER_API

namespace ARRanger
{

namespace Battle
{
  /**前方宣言 */
  struct FARDamageResult;

  /**
   * @brief ARRangerプロジェクト戦闘タスク構造体
   */
  struct FARBattleTask
  {
    /**攻撃元 */
    TObjectPtr<AActor> Instigator = nullptr;
    
    /**攻撃先 */
    TObjectPtr<AActor> Target = nullptr;
    
    /**ダメージ */
    float OriginDamage = 0.0f;
  };

  class IBattleSystemInterface
  {
    public:
      UE_API virtual ~IBattleSystemInterface() = default;

      /**
       * @brief バトルシステムを返す
       * @return IBattleSystemInterface& （シングルトン）
       */
      UE_API static IBattleSystemInterface& Get();
    
      /**
       * ※Pure virtual function※
       * @brief 戦闘タスクを処理する(純粋仮想関数)
       * 
       * @param Task      @see ARRanger::Battle::FARBattleTask
       * @param OutResult バトル結果構造体
       */
      UE_API virtual void HandleBattleTask(const ARRanger::Battle::FARBattleTask& Task, ARRanger::Battle::FARDamageResult& OutResult) = 0;
  };

} // namespace ARRanger::Battle

} // namespace ARRanger

#undef UE_API

#endif // _ARRANGER_BATTLE_SYSTEM_INTERFACE_

