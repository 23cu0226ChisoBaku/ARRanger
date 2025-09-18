
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
  if (AttackMontage == nullptr)
  {
    return EBTNodeResult::Failed;
  }

  AAIController* AIController = OwnerComp.GetAIOwner();
  if (AIController != nullptr)
  {
    if (ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn()))
    {
      if (UAnimInstance* AnimInstance = AICharacter->GetMesh()->GetAnimInstance())
      {
        AnimInstance->Montage_Play(AttackMontage);
  
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &UBTT_PunchAttack::OnMontageEnded, &OwnerComp);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
  
        return EBTNodeResult::InProgress;       
      }
    }
  }

  return EBTNodeResult::Failed;
}

void UBTT_PunchAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
  FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}