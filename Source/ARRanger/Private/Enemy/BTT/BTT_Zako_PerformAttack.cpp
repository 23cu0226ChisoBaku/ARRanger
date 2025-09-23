
#include "Enemy/BTT/BTT_Zako_PerformAttack.h"
#include "AIController.h"
#include "Enemy/Enemy_Zako.h"

EBTNodeResult::Type UBTT_Zako_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  AAIController* AIController = OwnerComp.GetAIOwner();
  AEnemy_Zako* Enemy = Cast<AEnemy_Zako>(AIController ? AIController->GetPawn() : nullptr);
  if (Enemy == nullptr)
  {
    return EBTNodeResult::Failed;
  } 

  Enemy->Zako_PerformAttack();
  return EBTNodeResult::Succeeded;
}
