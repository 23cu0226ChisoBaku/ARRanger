#include "BattleSystem/IBattleSystemInterface.h"

#include "BattleSystem/IARAttackable.h"
#include "BattleSystem/IARAttackerInterface.h"
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
  , LaunchDirection{EForceInit::ForceInitToZero}
  , Damage{0.0f}
  , bUseAttackerActor{true}
{ }

const FARAttackParameters FARAttackParameters::BlankAttackParams = FARAttackParameters{};

bool IARAttackable::IsActorAttackable(const AActor* InActor)
{
  if (InActor == nullptr)
  {
    return false;
  }

  return InActor->GetClass()->ImplementsInterface(UARAttackable::StaticClass());
}

// Add default functionality here for any IIARAttackable functions that are not pure virtual.
bool IARAttackable::AttackTarget(IARAttackerInterface* Attacker, FARAttackParameters InAttackParams)
{
  using namespace ARRanger::Battle;

  // Attackerが存在しないため、Attackerへの通知を送らない
  if (Attacker == nullptr)
  {
    AR_LOG(LogARBattle, Error, TEXT("Attacker is INVALID!!!"));
  }

  /**Preattack Phase */

  // Use the actor of IARAttackerInterface if we do not use the Instigator
  const bool bUseInstigator = (InAttackParams.Instigator != nullptr) && InAttackParams.bUseAttackerActor;
  if (!bUseInstigator)
  {
    InAttackParams.Instigator = Attacker != nullptr ? Attacker->GetActor() : nullptr;
  }

  FARAttackResult outAttackResult{};
  OnPreAttacked(InAttackParams, outAttackResult);

  /**攻撃者に攻撃結果を通知する */
  if (Attacker != nullptr)
  {
    FARAttackNotifyParameter notifyParams{};
    notifyParams.WeakAttackableObject = ::Cast<UObject>(this);
    Attacker->NotifyAttackResult(outAttackResult.Result, notifyParams);
  }

  /**Postattack Phase */
  OnPostAttacked(InAttackParams);
  
  /**Handle battle task */
  // 攻撃が失敗したらバトルタスクを処理しない
  if (outAttackResult.Result != EARAttackResult::Success)
  {
    return false;
  }

  // バトルタスクを処理する
  FARBattleTask task{};
  task.Instigator = InAttackParams.Instigator;
  task.Target = this->Attackable_GetActor();
  task.OriginDamage = InAttackParams.Damage;

  FARDamageResult damageResult{};
  IBattleSystemInterface::Get().HandleBattleTask(task, damageResult);

  // ダメージ通知
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
  // FIXME Need Implementation to handle task
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
