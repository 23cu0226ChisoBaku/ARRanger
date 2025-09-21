#include "Enemy/BTT/BTT_SelectAttack.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_SelectAttack::UBTT_SelectAttack()
{
  NodeName = TEXT("Choose Attack");
}

EBTNodeResult::Type UBTT_SelectAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
  if (BB == nullptr)
  {
    return EBTNodeResult::Failed;
  }

  EAttackType ChosenAttack = EAttackType::None;
  EAttackType LastAttack = (EAttackType)BB->GetValueAsEnum("LastAttackType");

  if (WeightedAttacks.Num() > 0)
  {
    float TotalWeight = 0.f;
    for (const auto& [ attackType, weight ] : WeightedAttacks)
    {
      if ((attackType == EAttackType::Roar) && (LastAttack == EAttackType::Roar))
      {
        continue;
      }

      TotalWeight += weight;
    }

    if (TotalWeight > 0.f)
    {
      const float Rand = FMath::FRandRange(0.f, TotalWeight);
      float Accum = 0.f;

      for (const auto& [ attackType, weight ] : WeightedAttacks)
      {
        if ((attackType == EAttackType::Roar) && (LastAttack == EAttackType::Roar))
        {
          continue;
        }

        Accum += weight;
        if (Rand <= Accum)
        {
          ChosenAttack = attackType;
          break;
        }
      }
    }
  }
  else if (CandidateAttacks.Num() > 0)
  {
    TArray<EAttackType> Filtered;
    for (const EAttackType& Attack : CandidateAttacks)
    {
      if (!(Attack == EAttackType::Roar && LastAttack == EAttackType::Roar))
      {
        Filtered.Add(Attack);
      }
    }

    if (Filtered.Num() > 0)
    {
      int32 Index = FMath::RandRange(0, Filtered.Num() - 1);
      ChosenAttack = Filtered[Index];
    }
    else
    {
      ChosenAttack = EAttackType::Roar;
    }
  }
  else
  {
    return EBTNodeResult::Failed;
  }

  // TODO Perform only punch for test purpose
  BB->SetValueAsEnum("AttackType", static_cast<uint8>(ChosenAttack));
  BB->SetValueAsEnum("LastAttackType", static_cast<uint8>(ChosenAttack));

  return EBTNodeResult::Succeeded;

}
