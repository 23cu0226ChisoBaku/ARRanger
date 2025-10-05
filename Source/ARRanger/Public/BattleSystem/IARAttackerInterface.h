/**
 * @file IARAttackerInterface.h
 * @brief Interface for attacker to receive attack result
 */

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
   * @brief Parameter structure of attack notify
   */
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

UINTERFACE(MinimalAPI)
class UARAttackerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * @class IARAttackerInterface
 * @brief Interface for attacker to receive attack result
 */
class IARAttackerInterface
{
	GENERATED_BODY()

public:

  /**
   * @brief   攻撃者のアクターを返す
   * @return  AActor
   */
  UE_API virtual AActor* GetActor() { return ::Cast<AActor>(_getUObject()); }

  // TODO Not implemented
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

  /**
   * @brief 攻撃が成功した時のコールバック
   * 
   * @param InNotifyParams @see ARRanger::Battle::FARAttackNotifyParameter
   */
  UE_API virtual void OnNotifyAttackResult_Success(const ARRanger::Battle::FARAttackNotifyParameter& InNotifyParams) { };

  /**
   * @brief 攻撃が失敗した時のコールバック
   * 
   * @param Reason          失敗の原因（戦闘結果列挙）
   * @param InNotifyParams  @see ARRanger::Battle::FARAttackNotifyParameter
   */
  UE_API virtual void OnNotifyAttackResult_Failed(ARRanger::Battle::EARAttackResult Reason, const ARRanger::Battle::FARAttackNotifyParameter& InNotifyParams) { };

};

#undef UE_API
