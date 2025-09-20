#include "Enemy/BTT/BTT_PerformAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Enemy/Enemy_MiddleBoss.h"

UBTT_PerformAttack::UBTT_PerformAttack()
{
  NodeName = TEXT("Perform Attack");
  bNotifyTick = true;
}

EBTNodeResult::Type UBTT_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  if ((OwnerComp.GetAIOwner() == nullptr) ||
      (OwnerComp.GetBlackboardComponent() == nullptr)
    )
  {
    return EBTNodeResult::Failed;
  }
  
  CachedOwnerComp = &OwnerComp;
  CachedAICon = OwnerComp.GetAIOwner();
  CachedBB = OwnerComp.GetBlackboardComponent();

  Boss = ::Cast<AEnemy_MiddleBoss>(CachedAICon->GetPawn());
  if (Boss == nullptr)
  {
    return EBTNodeResult::Failed;
  } 

  AActor* targetActor = Cast<AActor>(CachedBB->GetValueAsObject("TargetActor"));
  if (targetActor == nullptr)
  {
    return EBTNodeResult::Failed;
  } 

  Boss->SetTargetActor(targetActor);
  Boss->OnPreAttackTaskFinishedEvent.AddUObject(this, &ThisClass::OnPreAttackFinished);
  Boss->OnAttackFinished.AddUniqueDynamic(this, &ThisClass::OnAttackFinished);

  EAttackType AttackType = (EAttackType)CachedBB->GetValueAsEnum("AttackType");
  switch (AttackType)
  {
    // 攻撃
    case EAttackType::Punch:
    {
      CachedAICon->MoveToActor(targetActor, Boss->PunchRange, true, true, true, nullptr, true);
    }
    break;

    case EAttackType::JumpAttack:
    {

    }
    break;

    case EAttackType::Roar:
    {

    }
    break;

    case EAttackType::Slammed:
    {

    }
    break;
  }

  // Notify Boss to perform an attack
  Boss->OnAttackPerformed(AttackType);

  return EBTNodeResult::InProgress;
}

void UBTT_PerformAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
  if ((CachedAICon == nullptr) || (Boss == nullptr))
  {
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return;
  } 

  if (CachedOwnerComp == nullptr)
  {
    CachedOwnerComp = &OwnerComp;
  }

  Boss->UpdatePreAttack(DeltaSeconds);
}

void UBTT_PerformAttack::OnPreAttackFinished(EAttackType InType)
{
  switch (InType)
  {
    case EAttackType::Punch:
    {
      if (CachedAICon != nullptr)
      {
        CachedAICon->StopMovement();
      }
    }
    break;
  }

  if (Boss != nullptr)
  {
    Boss->OnPreAttackTaskFinishedEvent.RemoveAll(this);
  }
}

void UBTT_PerformAttack::OnAttackFinished()
{
  if (CachedOwnerComp != nullptr)
  {
    FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
  }

  if (Boss != nullptr)
  {
    Boss->OnAttackFinished.RemoveDynamic(this, &ThisClass::OnAttackFinished);
  }
}

