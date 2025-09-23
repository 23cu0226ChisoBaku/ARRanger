// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "IARBattleNotifyHandler.generated.h"

#define UE_API ARRANGER_API

enum struct EARBattleState
{
  StartBattle,
  EndBattle,
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UARBattleNotifyHandler : public UInterface
{
	GENERATED_BODY()
};

class IARBattleNotifyHandler
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

  /**
   * @brief     戦闘状態通知を送る
   */
  UE_API virtual void NotifyBattleState(EARBattleState InState) {}

  /**
   * @brief     戦闘状態中かを確認する
   * @return    boolean
   */
  UE_API virtual bool IsInBattle() const { return false; }

};

#undef UE_API