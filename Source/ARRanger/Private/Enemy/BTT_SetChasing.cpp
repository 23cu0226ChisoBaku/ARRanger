
#include "Enemy/BTT/BTT_SetChasing.h"
#include "Enemy/EnemyAnimInstance.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIController.h"

UBTT_SetChasing::UBTT_SetChasing()
{
  NodeName = TEXT("Set Chasing State");
}

EBTNodeResult::Type UBTT_SetChasing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  if (AAIController* AICon = OwnerComp.GetAIOwner())
  {
    if (ACharacter* Char = Cast<ACharacter>(AICon->GetPawn()))
    {
      if (USkeletalMeshComponent* Mesh = Char->GetMesh())
      {
        if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(Mesh->GetAnimInstance()))
        {
          EnemyAnim->bIsChasing = bChasing;
          EnemyAnim->bIsSearch = bIsSearch;
          return EBTNodeResult::Succeeded;
        }
      }

    }
  }

  return EBTNodeResult::Failed;
}
