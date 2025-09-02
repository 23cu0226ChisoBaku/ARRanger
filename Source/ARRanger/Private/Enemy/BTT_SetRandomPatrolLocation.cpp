

#include "Enemy/BTT/BTT_SetRandomPatrolLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

UBTT_SetRandomPatrolLocation::UBTT_SetRandomPatrolLocation()
{
	NodeName = TEXT("PatrolPosition");
}

EBTNodeResult::Type UBTT_SetRandomPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
    {
        return EBTNodeResult::Failed;
    }

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    FVector Origin = BlackboardComp->GetValueAsVector(PatrolOriginKey.SelectedKeyName);

    // デバッグ用: Originの値をログに表示
    UE_LOG(LogTemp, Warning, TEXT("Current Origin: %s"), *Origin.ToString());

    // 初期値がゼロベクトルなら現在位置をOriginとして設定
    if (Origin.IsZero())
    {
        Origin = AIPawn->GetActorLocation();
        BlackboardComp->SetValueAsVector(PatrolOriginKey.SelectedKeyName, Origin);
        UE_LOG(LogTemp, Warning, TEXT("Start Location Set: %s"), *Origin.ToString());  // 追加
    }

    FNavLocation RandomPatrolLocation;

    if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, RandomPatrolLocation))
    {
        UE_LOG(LogTemp, Warning, TEXT("Succeeded Location: %s"), *RandomPatrolLocation.Location.ToString());
        BlackboardComp->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomPatrolLocation.Location);
        return EBTNodeResult::Succeeded;
    }

    UE_LOG(LogTemp, Warning, TEXT("Failed Location"));
    return EBTNodeResult::Failed;
}
