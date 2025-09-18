
#include "Enemy/BTT/BTT_SetAttackCooldown.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TimerManager.h"
#include "AIController.h"

UBTT_SetAttackCooldown::UBTT_SetAttackCooldown()
{
	NodeName = TEXT("Set Attack Cooldown");
  // Use this to safe TimerHandler for every node
  bCreateNodeInstance = true;

  m_coolDownTimerHandle.Invalidate();
}

EBTNodeResult::Type UBTT_SetAttackCooldown::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
  if (BB == nullptr)
  {
    return EBTNodeResult::Failed;
  } 

  BB->SetValueAsBool(TEXT("CanAttack"), false);

  OwnerComp.GetWorld()->GetTimerManager().SetTimer(m_coolDownTimerHandle, [BB]()
    {
      BB->SetValueAsBool(TEXT("CanAttack"), true);
    }, CooldownTime, false);

  return EBTNodeResult::Succeeded;
}

void UBTT_SetAttackCooldown::OnInstanceDestroyed(UBehaviorTreeComponent& OwnerComp)
{
  if (m_coolDownTimerHandle.IsValid())
  {
    OwnerComp.GetWorld()->GetTimerManager().ClearTimer(m_coolDownTimerHandle);
    m_coolDownTimerHandle.Invalidate();
  }
}