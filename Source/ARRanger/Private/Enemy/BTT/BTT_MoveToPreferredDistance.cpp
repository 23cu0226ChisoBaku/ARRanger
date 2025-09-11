#include "Enemy/BTT/BTT_MoveToPreferredDistance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"

UBTT_MoveToPreferredDistance::UBTT_MoveToPreferredDistance()
{
    NodeName = TEXT("Move To Preferred Distance");
    bNotifyTick = true; // Tickで監視する
    bIsMoving = false;
}

EBTNodeResult::Type UBTT_MoveToPreferredDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
    APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();

    if (!Target || !ControlledPawn) return EBTNodeResult::Failed;

    float PreferredDistance = BB->GetValueAsFloat("PreferredDistance");
    FVector ToTarget = ControlledPawn->GetActorLocation() - Target->GetActorLocation();
    float CurrentDistance = ToTarget.Size();

    // すでに距離が近いなら移動不要
    if (FMath::Abs(CurrentDistance - PreferredDistance) < 100.f)
    {
        return EBTNodeResult::Succeeded;
    }

    ToTarget.Normalize();
    FVector DesiredLocation = Target->GetActorLocation() + ToTarget * PreferredDistance;

    // NavMesh補正
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    FNavLocation Projected;
    if (NavSys && NavSys->ProjectPointToNavigation(DesiredLocation, Projected))
    {
        CurrentMoveGoal = Projected.Location;
        OwnerComp.GetAIOwner()->MoveToLocation(CurrentMoveGoal, 50.0f); // 半径50で到達判定
        bIsMoving = true;
        return EBTNodeResult::InProgress;
    }

    return EBTNodeResult::Failed;
}

void UBTT_MoveToPreferredDistance::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AICon ? AICon->GetPawn() : nullptr;
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!AICon || !ControlledPawn || !BB)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AActor* Target = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
    if (!Target)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    float PreferredDistance = BB->GetValueAsFloat("PreferredDistance");
    FVector ToTarget = ControlledPawn->GetActorLocation() - Target->GetActorLocation();
    float CurrentDistance = ToTarget.Size();

    // 目標距離に収まったらタスク終了
    if (FMath::Abs(CurrentDistance - PreferredDistance) < 100.f)
    {
        AICon->StopMovement();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // ゴールを再計算
    ToTarget.Normalize();
    FVector DesiredLocation = Target->GetActorLocation() + ToTarget * PreferredDistance;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    FNavLocation Projected;
    if (NavSys && NavSys->ProjectPointToNavigation(DesiredLocation, Projected))
    {
        AICon->MoveToLocation(Projected.Location, 50.f, false);
    }
}
