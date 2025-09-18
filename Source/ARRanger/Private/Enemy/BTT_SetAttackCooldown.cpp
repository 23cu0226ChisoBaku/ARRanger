
#include "Enemy/BTT/BTT_SetAttackCooldown.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TimerManager.h"
#include "AIController.h"

UBTT_SetAttackCooldown::UBTT_SetAttackCooldown()
{
	NodeName = TEXT("Set Attack Cooldown");
}

EBTNodeResult::Type UBTT_SetAttackCooldown::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
  if (BB == nullptr)
  {
    return EBTNodeResult::Failed;
  } 

  BB->SetValueAsBool(TEXT("CanAttack"), false);

  FTimerHandle dummy{};
  OwnerComp.GetWorld()->GetTimerManager().SetTimer(dummy, [BB]()
      {
          BB->SetValueAsBool(TEXT("CanAttack"), true);
      }, CooldownTime, false);

  return EBTNodeResult::Succeeded;
}