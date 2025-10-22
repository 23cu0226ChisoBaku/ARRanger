// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "IARAttackerInterface.generated.h"

#define UE_API ARRANGER_API

/**Forwar declaration */
class IARBattleNotifyHandler;

namespace ARRanger
{

namespace Battle
{
  /**Forward declaration */
  enum struct EARAttackResult : uint8;

  /**
   * @brief 攻撃通知パラメータ
   */
  struct FARAttackNotifyParameter
  {
    /**攻撃ターゲットのUObject */
    TWeakObjectPtr<> WeakAttackableObject = nullptr;
  };

} // namespace ARRanger::Battle

} // namespace ARRanger


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UARAttackerInterface : public UInterface
{
	GENERATED_BODY()
};

class IARAttackerInterface
{
	GENERATED_BODY()

public:

  /**
   * @brief   攻撃者のアクターを返す
   * @return  AActor
   */
  UE_API virtual AActor* GetActor() { return ::Cast<AActor>(_getUObject()); }

  /**
   * @brief  バトル通知ハンドラーを返す
   * @return IARBattleNotifyHandler (Maybe null)
   */
  UE_API virtual IARBattleNotifyHandler* GetBattleNotifyHandler() const { return nullptr; }
  
  /**
   * @brief                 攻撃結果通知送る
   * @param InResult        攻撃結果(列挙型)
   * @param InNotifyParams  通知パラメータ
   */
  UE_API void NotifyAttackResult(ARRanger::Battle::EARAttackResult InResult, const ARRanger::Battle::FARAttackNotifyParameter& InNotifyParams);
  
protected:

  /**攻撃が成功した時のコールバック */
  UE_API virtual void OnNotifyAttackResult_Success(const ARRanger::Battle::FARAttackNotifyParameter& InNotifyParams) { };

  /**攻撃が失敗した時のコールバック */
  UE_API virtual void OnNotifyAttackResult_Failed(ARRanger::Battle::EARAttackResult Reason, const ARRanger::Battle::FARAttackNotifyParameter& InNotifyParams) { };

};

#undef UE_API
