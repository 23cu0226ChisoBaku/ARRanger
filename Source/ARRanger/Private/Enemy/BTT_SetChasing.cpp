
#include "Enemy/BTT/BTT_SetChasing.h"
#include "Enemy/EnemyAnimInstance.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "AIController.h"

UBTT_SetChasing::UBTT_SetChasing()
{
    NodeName = TEXT("Set Chasing State");
}

/*EBTNodeResult::Type UBTT_SetChasing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (AAIController* AICon = OwnerComp.GetAIOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("AISucceed"));
        if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(AICon->GetPawn()))
        {
            EnemyAnim->bIsChasing = bChasing;
            EnemyAnim->bIsSearch = bIsSearch;
            UE_LOG(LogTemp, Warning, TEXT("Succeeded"));
            return EBTNodeResult::Succeeded;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Failed"));
    return EBTNodeResult::Failed;
}*/

EBTNodeResult::Type UBTT_SetChasing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (AAIController* AICon = OwnerComp.GetAIOwner())
    {
        APawn* Pawn = AICon->GetPawn();
        if (!Pawn)
        {
            UE_LOG(LogTemp, Warning, TEXT("SetChasing: Pawn is null"));
            return EBTNodeResult::Failed;
        }

        // ÇŸÇ∆ÇÒÇ«ÇÃèÍçáÇÕ ACharacter Ç»ÇÃÇ≈Ç‹Ç∏ÇÕÉLÉÉÉXÉg
        if (ACharacter* Char = Cast<ACharacter>(Pawn))
        {
            USkeletalMeshComponent* Mesh = Char->GetMesh();
            if (!Mesh)
            {
                UE_LOG(LogTemp, Warning, TEXT("SetChasing: Mesh is null (Character has no mesh)"));
                return EBTNodeResult::Failed;
            }

            if (UEnemyAnimInstance* EnemyAnim = Cast<UEnemyAnimInstance>(Mesh->GetAnimInstance()))
            {
                EnemyAnim->bIsChasing = bChasing;
                EnemyAnim->bIsSearch = bIsSearch;
                UE_LOG(LogTemp, Warning, TEXT("SetChasing: Succeeded (AnimInstance set)"));
                return EBTNodeResult::Succeeded;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("SetChasing: AnimInstance is not UEnemyAnimInstance"));
                return EBTNodeResult::Failed;
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("SetChasing: Failed"));
    return EBTNodeResult::Failed;
}
