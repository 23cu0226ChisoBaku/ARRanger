// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef _ARRANGER_BATTLE_SYSTEM_INTERFACE_
#define _ARRANGER_BATTLE_SYSTEM_INTERFACE_

namespace ARRanger
{

namespace Battle
{
  struct FARDamageResult;

  struct FARBattleTask
  {
    TObjectPtr<AActor> Instigator = nullptr;
    TObjectPtr<AActor> Target = nullptr;
    float OriginDamage = 0.0f;
  };

  /**
   * @brief
   */
  class IBattleSystemInterface
  {
  
  public:
    ARRANGER_API ~IBattleSystemInterface() = default;

    ARRANGER_API static IBattleSystemInterface& Get();
  
    ARRANGER_API virtual void HandleBattleTask(const FARBattleTask& Task, FARDamageResult& OutResult) = 0;
  
  };

} // namespace ARRanger::Battle

} // namespace ARRanger

#endif // _ARRANGER_BATTLE_SYSTEM_INTERFACE_

