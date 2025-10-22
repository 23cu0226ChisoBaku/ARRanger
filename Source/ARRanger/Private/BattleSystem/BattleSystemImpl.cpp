// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleSystem/IBattleSystemInterface.h"

#include "BattleSystem/IARAttackable.h"
#include "BattleSystem/IARAttackerInterface.h"
#include "BattleSystem/IARBattleNotifyHandler.h"
#include "BattleSystem/IBattleSystemInterface.h"

/**Internal use */
#include "Internal/ARLoggingHeader.h"

class FARBattleSystem final : public ARRanger::Battle::IBattleSystemInterface
{ 
  public:
    static FARBattleSystem& Get();

    void HandleBattleTask(const ARRanger::Battle::FARBattleTask& Task, ARRanger::Battle::FARDamageResult& OutResult) override final;
};

/**Start of IARAttackable Interface Implementation */
#pragma region IARAttackable Interface

FARAttackParameters::FARAttackParameters()
  : Instigator{nullptr}
  , Damage{0.0f}
  , LaunchDirection{EForceInit::ForceInitToZero}
  , bUseAttackerActor{true}
{ }

const FARAttackParameters FARAttackParameters::BlankAttackParams = FARAttackParameters{};

// Add default functionality here for any IIARAttackable functions that are not pure virtual.
bool IARAttackable::AttackTarget(IARAttackerInterface* Attacker, FARAttackParameters InAttackParams)
{
  // Attackerが存在しないため、Attackerへの通知を送らない
  if (Attacker == nullptr)
  {
    AR_LOG(LogARBattle, Error, TEXT("Attacker is INVALID!!!"));
  }
  else
  {
    // TODO Should we notify here? 
    if (IARBattleNotifyHandler* notifyHandler = Attacker->GetBattleNotifyHandler())
    {
      notifyHandler->NotifyBattleState(EARBattleState::StartBattle);
    }
  }

  /**Preattack Phase */
  // Use Instigator if Instigator is valid and bUseAttackerActor is false, otherwise use actor of IARAttackerInterface
  const bool bUseIntigator = (InAttackParams.Instigator != nullptr) && InAttackParams.bUseAttackerActor;
  if (!bUseIntigator)
  {
    InAttackParams.Instigator = Attacker != nullptr ? Attacker->GetActor() : nullptr;
  }

  // Check attack result
  ARRanger::Battle::FARAttackResult outAttackResult{};
  OnPreAttacked(InAttackParams, outAttackResult);

  /**Notify attack state to attacker */
  if (Attacker != nullptr)
  {
    ARRanger::Battle::FARAttackNotifyParameter notifyParams{};
    notifyParams.WeakAttackableObject = _getUObject();
    Attacker->NotifyAttackResult(outAttackResult.Result, notifyParams);
  }

  /**Postattack Phase */
  OnPostAttacked(InAttackParams);
  
  /**Handle battle task */
  // Do not handle battle task if attack is not success
  if (outAttackResult.Result != ARRanger::Battle::EARAttackResult::Success)
  {
    return false;
  }

  // Finally we handle battle task
  ARRanger::Battle::IBattleSystemInterface& battleSystem = ARRanger::Battle::IBattleSystemInterface::Get();
  ARRanger::Battle::FARBattleTask task{};
  ARRanger::Battle::FARDamageResult damageResult{};
  task.Instigator = InAttackParams.Instigator;
  task.Target = this->Attackable_GetActor();
  task.OriginDamage = InAttackParams.Damage;
  battleSystem.HandleBattleTask(task, damageResult);

  /**Handle damage */
  damageResult.FinalLaunchDirection = InAttackParams.LaunchDirection;
  damageResult.Instigator = (Attacker != nullptr) ? Attacker->GetActor() : nullptr;
  damageResult.ImpactLocation = InAttackParams.ImpactLocation;
  OnDamaged(damageResult);

  // Attack success
  return true;
}

#pragma endregion IARAttackable Interface
/**End of IARAttackable Interface Implementation */

/**Start of IARAttackerInterface Interface Implementation */
#pragma region IARAttackerInterface Interface

void IARAttackerInterface::NotifyAttackResult(ARRanger::Battle::EARAttackResult InResult, const ARRanger::Battle::FARAttackNotifyParameter& InNotifyParams)
{
  if (InResult == ARRanger::Battle::EARAttackResult::Success)
  {
    OnNotifyAttackResult_Success(InNotifyParams);
  }
  else
  {
    OnNotifyAttackResult_Failed(InResult, InNotifyParams);
  }
}

#pragma endregion IARAttackerInterface Interface
/**End of IARAttackerInterface Interface Implementation */

FARBattleSystem& FARBattleSystem::Get()
{
  static FARBattleSystem Instance;
  return Instance;
}

void FARBattleSystem::HandleBattleTask(const ARRanger::Battle::FARBattleTask& Task, ARRanger::Battle::FARDamageResult& OutResult)
{
  // TODO Need Implementation to handle task
  OutResult.FinalDamage = Task.OriginDamage;
}

namespace ARRanger
{

namespace Battle
{

  IBattleSystemInterface& IBattleSystemInterface::Get()
  {
    return FARBattleSystem::Get();
  }

} // namespace ARRanger::Battle

} // namespace ARRanger
