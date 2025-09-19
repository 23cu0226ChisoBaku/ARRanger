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
    bHasStartedJump = false;
    bIsMovingToTarget = false;
}

EBTNodeResult::Type UBTT_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
  if (OwnerComp.GetAIOwner() == nullptr ||
  OwnerComp.GetBlackboardComponent() == nullptr)
  {
    return EBTNodeResult::Failed;
  }
  
  CachedOwnerComp = &OwnerComp;
  CachedAICon = OwnerComp.GetAIOwner();
  CachedBB = OwnerComp.GetBlackboardComponent();

  ACharacter* Boss = ::Cast<ACharacter>(CachedAICon->GetPawn());
  if (Boss == nullptr)
  {
    return EBTNodeResult::Failed;
  } 

  TargetActor = Cast<AActor>(CachedBB->GetValueAsObject("TargetActor"));
  if (TargetActor == nullptr)
  {
    return EBTNodeResult::Failed;
  } 

  EAttackType AttackType = (EAttackType)CachedBB->GetValueAsEnum("AttackType");

  switch (AttackType)
  {
    // 攻撃
    case EAttackType::Punch:
    {
      if (PunchMontage == nullptr)
      {
        return EBTNodeResult::Failed;
      } 

      // Get direction to target
      FVector Dir = (TargetActor->GetActorLocation() - Boss->GetActorLocation());
      Dir.Z = 0.f;
      if (!Dir.IsNearlyZero())
      {
        Boss->SetActorRotation(Dir.Rotation());
      }

      float Distance = FVector::Dist(Boss->GetActorLocation(), TargetActor->GetActorLocation());
      if (Distance > PunchRange)
      {
        bIsMovingToTarget = true;
        CachedAICon->MoveToActor(TargetActor, PunchRange, true, true, true, nullptr, true);
        return EBTNodeResult::InProgress;
      }
      else
      {
        bIsMovingToTarget = false;
        CachedAICon->StopMovement();
        return PlayAttackMontage(Boss, PunchMontage, AttackType);
      }
    }

    case EAttackType::JumpAttack:
    {
      if (!JumpAttackMontage) return EBTNodeResult::Failed;

      // ��������
      FVector Dir = (TargetActor->GetActorLocation() - Boss->GetActorLocation());
      Dir.Z = 0.f;
      if (!Dir.IsNearlyZero())
          Boss->SetActorRotation(Dir.Rotation());

      // �W�����v�^�[�Q�b�g���Œ�
      JumpTargetLocation = TargetActor->GetActorLocation();
      bHasStartedJump = true;

      return PlayAttackMontage(Boss, JumpAttackMontage, AttackType);
    }

    case EAttackType::Roar:
    {
      if (!RoarMontage) return EBTNodeResult::Failed;

      // ��������
      FVector Dir = (TargetActor->GetActorLocation() - Boss->GetActorLocation());
      Dir.Z = 0.f;
      if (!Dir.IsNearlyZero())
          Boss->SetActorRotation(Dir.Rotation());

      return PlayAttackMontage(Boss, RoarMontage, AttackType);
    }

    case EAttackType::Slammed:
    {
      if (!SlammedMontage) return EBTNodeResult::Failed;

      // ��������
      FVector Dir = (TargetActor->GetActorLocation() - Boss->GetActorLocation());
      Dir.Z = 0.f;
      if (!Dir.IsNearlyZero())
          Boss->SetActorRotation(Dir.Rotation());

      return PlayAttackMontage(Boss, SlammedMontage, AttackType);
    }
  }

  return EBTNodeResult::Failed;
}

void UBTT_PerformAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (!CachedAICon || !TargetActor) return;

    ACharacter* Boss = Cast<ACharacter>(CachedAICon->GetPawn());
    if (!Boss) return;

    EAttackType AttackType = (EAttackType)CachedBB->GetValueAsEnum("AttackType");

    // 1. �ʏ�U���i�p���`�j�O�i����
    if (AttackType == EAttackType::Punch && bIsMovingToTarget)
    {
        FVector Dir = (TargetActor->GetActorLocation() - Boss->GetActorLocation());
        Dir.Z = 0.f;
        if (!Dir.IsNearlyZero())
            Boss->SetActorRotation(Dir.Rotation());

        float Distance = FVector::Dist(Boss->GetActorLocation(), TargetActor->GetActorLocation());

        // �v���C���[�Ƃ̋������U���͈͓��Ȃ�U��
        if (Distance <= PunchRange)
        {
            bIsMovingToTarget = false;
            CachedAICon->StopMovement();
            PlayAttackMontage(Boss, PunchMontage, AttackType);
        }
        else
        {
            // �ړ����͑��x����AnimBP�ɓn��
            AEnemy_MiddleBoss* BossChar = Cast<AEnemy_MiddleBoss>(Boss);
            if (BossChar)
            {
                BossChar->CurrentSpeed = PunchMoveSpeed; 
            }
        }
    }

    // 2. �W�����v�U�� �O�i����iXY�̂݁j
    if (AttackType == EAttackType::JumpAttack && bHasStartedJump)
    {
        FVector Dir = (JumpTargetLocation - Boss->GetActorLocation()).GetSafeNormal2D();
        float Distance = FVector::Dist2D(Boss->GetActorLocation(), JumpTargetLocation);

        if (Distance > 50.f) // ��~����
        {
            Boss->AddMovementInput(Dir, JumpMoveSpeed * DeltaSeconds);

            // �ړ����͕����A�j���[�V�������Đ�
            AEnemy_MiddleBoss* BossChar = Cast<AEnemy_MiddleBoss>(Boss);
            if (BossChar)
            {
                BossChar->CurrentSpeed = JumpMoveSpeed;
            }
        }
        else
        {
            bHasStartedJump = false;

            // ��~���͑��x���[����
            AEnemy_MiddleBoss* BossChar = Cast<AEnemy_MiddleBoss>(Boss);
            if (BossChar)
            {
                BossChar->CurrentSpeed = 0.f;
            }
        }
    }
}

EBTNodeResult::Type UBTT_PerformAttack::PlayAttackMontage(ACharacter* Boss, UAnimMontage* Montage, EAttackType AttackType)
{
  if ((Montage == nullptr) || (Boss == nullptr))
  {
    return EBTNodeResult::Failed;
  } 

  UAnimInstance* AnimInst = Boss->GetMesh()->GetAnimInstance();
  if (AnimInst == nullptr)
  {
    return EBTNodeResult::Failed;
  } 

  AnimInst->RootMotionMode = ERootMotionMode::IgnoreRootMotion;

  FOnMontageEnded EndDelegate;
  EndDelegate.BindUFunction(this, FName("OnAttackMontageEnded"));
  AnimInst->Montage_Play(Montage, 1.f);
  AnimInst->Montage_SetEndDelegate(EndDelegate, Montage);

  PerformAttackEffect(Boss, AttackType);

  return EBTNodeResult::InProgress;
}

void UBTT_PerformAttack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
  if (CachedAICon)
  {
    ACharacter* Boss = Cast<ACharacter>(CachedAICon->GetPawn());
    if (Boss && Boss->GetMesh() && Boss->GetMesh()->GetAnimInstance())
    {
        Boss->GetMesh()->GetAnimInstance()->RootMotionMode = ERootMotionMode::IgnoreRootMotion;
    }
  }

  if (CachedOwnerComp)
    FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}

void UBTT_PerformAttack::PerformAttackEffect(ACharacter* Boss, EAttackType AttackType)
{
    switch (AttackType)
    {
      case EAttackType::Punch:
          break;
      case EAttackType::JumpAttack:
          break;
      case EAttackType::Roar:
          break;
      default:
          break;
    }
}
