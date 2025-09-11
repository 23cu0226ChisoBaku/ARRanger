#include "Enemy/BTT/BTT_SetMoveLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

EBTNodeResult::Type UBTT_SetMoveLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    AActor* Target = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
    APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();

    if (!Target || !ControlledPawn)
    {
        return EBTNodeResult::Failed;
    }

    float PreferredDistance = BB->GetValueAsFloat("PreferredDistance");

    // 敵からプレイヤーへの方向ベクトル
    FVector ToTarget = ControlledPawn->GetActorLocation() - Target->GetActorLocation();

    if (!ToTarget.IsNearlyZero())
    {
        ToTarget.Normalize();

        // プレイヤーの位置からPreferredDistance離れた位置を算出
        FVector DesiredLocation = Target->GetActorLocation() + ToTarget * PreferredDistance;

        // NavMesh上に補正
        UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
        FNavLocation Projected;
        if (NavSys && NavSys->ProjectPointToNavigation(DesiredLocation, Projected))
        {
            BB->SetValueAsVector("MoveLocation", Projected.Location);
            UE_LOG(LogTemp, Warning, TEXT("移動先(補正後): %s"), *Projected.Location.ToString());
        }
        else
        {
            // NavMesh外なら現在位置に退避
            BB->SetValueAsVector("MoveLocation", ControlledPawn->GetActorLocation());
            UE_LOG(LogTemp, Error, TEXT("Invalid MoveLocation! 現在位置を代用"));
        }
    }

    return EBTNodeResult::Succeeded;
}
