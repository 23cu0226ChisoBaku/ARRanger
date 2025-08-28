
#include "Enemy/BTT/BTT_PunchAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

UBTT_PunchAttack::UBTT_PunchAttack()
{
	NodeName = TEXT("Punch Attack");
}

EBTNodeResult::Type UBTT_PunchAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());
    if (!AICharacter) return EBTNodeResult::Failed;

    if (UAnimInstance* AnimInstance = AICharacter->GetMesh()->GetAnimInstance())
    {
        if (AttackMontage)
        {
            AnimInstance->Montage_Play(AttackMontage);

            // デリゲートで終了検知
            FOnMontageEnded EndDelegate;
            EndDelegate.BindUObject(this, &UBTT_PunchAttack::OnMontageEnded, &OwnerComp);
            AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);

            return EBTNodeResult::InProgress; // ここでタスク継続
        }
    }
    return EBTNodeResult::Failed;
}

void UBTT_PunchAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
    FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded); // 再生完了後に成功扱い
}