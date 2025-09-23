
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
  if (AIController == nullptr)
  {
    return EBTNodeResult::Succeeded;
  }

  APawn* AIPawn = AIController->GetPawn();
  if (AIPawn == nullptr)
  {
    return EBTNodeResult::Succeeded;
  } 

  UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIController->GetWorld());
  if (NavSys == nullptr)
  {
    return EBTNodeResult::Succeeded;
  } 

  UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
  if (BlackboardComp == nullptr)
  {
    return EBTNodeResult::Succeeded;
  } 

  FVector Origin = BlackboardComp->GetValueAsVector(PatrolOriginKey.SelectedKeyName);
  if (!BlackboardComp->IsVectorValueSet(PatrolOriginKey.SelectedKeyName))
  {
    Origin = AIPawn->GetActorLocation();
    BlackboardComp->SetValueAsVector(PatrolOriginKey.SelectedKeyName, Origin);
  }

  FNavLocation RandomPatrolLocation{};
  if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, RandomPatrolLocation))
  {
    BlackboardComp->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomPatrolLocation.Location);
  }
  else
  {
    BlackboardComp->SetValueAsVector(PatrolLocationKey.SelectedKeyName, Origin);
  }

  return EBTNodeResult::Succeeded;
}
