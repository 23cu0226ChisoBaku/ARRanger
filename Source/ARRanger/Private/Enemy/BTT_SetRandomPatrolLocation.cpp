

#include "Enemy/BTT_SetRandomPatrolLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

UBTT_SetRandomPatrolLocation::UBTT_SetRandomPatrolLocation()
{
	NodeName = TEXT("PatrolPosition");
}

EBTNodeResult::Type UBTT_SetRandomPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 1. AIコントローラーとポーンを取得
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

	// 2. ナビゲーションシステムを取得
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		return EBTNodeResult::Failed;
	}

	// 3. 現在地からランダムなパトロール位置を検索
	FNavLocation RandomPatrolLocation;
	FVector Origin = AIPawn->GetActorLocation();

	if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, RandomPatrolLocation))
	{
		// 4. ブラックボードに新しいパトロール位置を保存
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomPatrolLocation.Location);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}