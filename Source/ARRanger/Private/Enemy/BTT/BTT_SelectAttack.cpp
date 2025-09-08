#include "Enemy/BTT/BTT_SelectAttack.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_SelectAttack::UBTT_SelectAttack()
{
    NodeName = TEXT("Choose Attack");
}

EBTNodeResult::Type UBTT_SelectAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    EAttackType ChosenAttack = EAttackType::None;

    // 前回の攻撃を取得
    EAttackType LastAttack = (EAttackType)BB->GetValueAsEnum("LastAttackType");

    // ▼ 1. WeightedAttacks が設定されていたら重み付きランダム選択
    if (WeightedAttacks.Num() > 0)
    {
        float TotalWeight = 0.f;
        for (const auto& Elem : WeightedAttacks)
        {
            // 前回と同じRoarは除外
            if (Elem.Key == EAttackType::Roar && LastAttack == EAttackType::Roar)
                continue;

            TotalWeight += Elem.Value;
        }

        if (TotalWeight > 0.f)
        {
            const float Rand = FMath::FRandRange(0.f, TotalWeight);
            float Accum = 0.f;

            for (const auto& Elem : WeightedAttacks)
            {
                if (Elem.Key == EAttackType::Roar && LastAttack == EAttackType::Roar)
                    continue;

                Accum += Elem.Value;
                if (Rand <= Accum)
                {
                    ChosenAttack = Elem.Key;
                    break;
                }
            }
        }
    }
    // ▼ 2. CandidateAttacks が設定されていたら単純ランダム選択
    else if (CandidateAttacks.Num() > 0)
    {
        TArray<EAttackType> Filtered;
        for (auto Attack : CandidateAttacks)
        {
            if (!(Attack == EAttackType::Roar && LastAttack == EAttackType::Roar))
                Filtered.Add(Attack);
        }

        if (Filtered.Num() > 0)
        {
            int32 Index = FMath::RandRange(0, Filtered.Num() - 1);
            ChosenAttack = Filtered[Index];
        }
        else
        {
            // 前回がRoarで他の候補がなければRoar選択
            ChosenAttack = EAttackType::Roar;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No attacks set in BTT_ChooseAttack"));
        return EBTNodeResult::Failed;
    }

    // Blackboardに書き込み
    BB->SetValueAsEnum("AttackType", static_cast<uint8>(ChosenAttack));
    BB->SetValueAsEnum("LastAttackType", static_cast<uint8>(ChosenAttack)); // 前回攻撃として保存

    return EBTNodeResult::Succeeded;
}
