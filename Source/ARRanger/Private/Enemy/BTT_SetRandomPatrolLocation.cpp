
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
    if (!AIController) return EBTNodeResult::Failed;

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn) return EBTNodeResult::Failed;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIController->GetWorld());
    if (!NavSys) return EBTNodeResult::Failed;

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return EBTNodeResult::Failed;

    FVector Origin = BlackboardComp->GetValueAsVector(PatrolOriginKey.SelectedKeyName);

    //
    if (!FMath::IsFinite(Origin.X) || !FMath::IsFinite(Origin.Y) || !FMath::IsFinite(Origin.Z))
    {
        Origin = AIPawn->GetActorLocation();
        BlackboardComp->SetValueAsVector(PatrolOriginKey.SelectedKeyName, Origin);
        UE_LOG(LogTemp, Warning, TEXT("Start Location Set: %s"), *Origin.ToString());
    }

    FNavLocation RandomPatrolLocation;
    if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, RandomPatrolLocation))
    {
        BlackboardComp->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomPatrolLocation.Location);

        UE_LOG(LogTemp, Warning, TEXT("New Patrol Location: %s"), *RandomPatrolLocation.Location.ToString());
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}
