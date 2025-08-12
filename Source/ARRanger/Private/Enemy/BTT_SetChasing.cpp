
#include "Enemy/BTT_SetChasing.h"
#include "Enemy/Enemy_Zako.h"
#include "AIController.h"

UBTT_SetChasing::UBTT_SetChasing()
{
    NodeName = TEXT("Set Chasing State");
}

EBTNodeResult::Type UBTT_SetChasing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (AAIController* AICon = OwnerComp.GetAIOwner())
    {
        if (AEnemy_Zako* Enemy = Cast<AEnemy_Zako>(AICon->GetPawn()))
        {
            Enemy->SetIsChasing(bChasing);
            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}