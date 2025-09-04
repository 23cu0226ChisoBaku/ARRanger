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
    CachedOwnerComp = &OwnerComp;
    CachedAICon = OwnerComp.GetAIOwner();
    if (!CachedAICon) return EBTNodeResult::Failed;

    ACharacter* Boss = Cast<ACharacter>(CachedAICon->GetPawn());
    if (!Boss) return EBTNodeResult::Failed;

    CachedBB = OwnerComp.GetBlackboardComponent();
    if (!CachedBB) return EBTNodeResult::Failed;

    TargetActor = Cast<AActor>(CachedBB->GetValueAsObject("TargetActor"));
    if (!TargetActor) return EBTNodeResult::Failed;

    EAttackType AttackType = (EAttackType)CachedBB->GetValueAsEnum("AttackType");

    switch (AttackType)
    {
    case EAttackType::Punch:
    {
        if (!PunchMontage) return EBTNodeResult::Failed;

        // プレイヤー方向に向く
        FVector Dir = (TargetActor->GetActorLocation() - Boss->GetActorLocation());
        Dir.Z = 0.f;
        if (!Dir.IsNearlyZero())
            Boss->SetActorRotation(Dir.Rotation());

        // 距離チェック
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

        // 向き調整
        FVector Dir = (TargetActor->GetActorLocation() - Boss->GetActorLocation());
        Dir.Z = 0.f;
        if (!Dir.IsNearlyZero())
            Boss->SetActorRotation(Dir.Rotation());

        // ジャンプターゲットを固定
        JumpTargetLocation = TargetActor->GetActorLocation();
        bHasStartedJump = true;

        return PlayAttackMontage(Boss, JumpAttackMontage, AttackType);
    }

    case EAttackType::Roar:
    {
        if (!RoarMontage) return EBTNodeResult::Failed;

        // 向き調整
        FVector Dir = (TargetActor->GetActorLocation() - Boss->GetActorLocation());
        Dir.Z = 0.f;
        if (!Dir.IsNearlyZero())
            Boss->SetActorRotation(Dir.Rotation());

        return PlayAttackMontage(Boss, RoarMontage, AttackType);
    }

    case EAttackType::Slammed:
    {
        if (!SlammedMontage) return EBTNodeResult::Failed;

        // 向き調整
        FVector Dir = (TargetActor->GetActorLocation() - Boss->GetActorLocation());
        Dir.Z = 0.f;
        if (!Dir.IsNearlyZero())
            Boss->SetActorRotation(Dir.Rotation());

        return PlayAttackMontage(Boss, SlammedMontage, AttackType);
    }

    default:
        return EBTNodeResult::Failed;
    }
}

void UBTT_PerformAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (!CachedAICon || !TargetActor) return;

    ACharacter* Boss = Cast<ACharacter>(CachedAICon->GetPawn());
    if (!Boss) return;

    EAttackType AttackType = (EAttackType)CachedBB->GetValueAsEnum("AttackType");

    // 1. 通常攻撃（パンチ）前進制御
    if (AttackType == EAttackType::Punch && bIsMovingToTarget)
    {
        FVector Dir = (TargetActor->GetActorLocation() - Boss->GetActorLocation());
        Dir.Z = 0.f;
        if (!Dir.IsNearlyZero())
            Boss->SetActorRotation(Dir.Rotation());

        float Distance = FVector::Dist(Boss->GetActorLocation(), TargetActor->GetActorLocation());

        // プレイヤーとの距離が攻撃範囲内なら攻撃
        if (Distance <= PunchRange)
        {
            bIsMovingToTarget = false;
            CachedAICon->StopMovement();
            PlayAttackMontage(Boss, PunchMontage, AttackType);
        }
        else
        {
            // 移動中は速度情報をAnimBPに渡す
            AEnemy_MiddleBoss* BossChar = Cast<AEnemy_MiddleBoss>(Boss);
            if (BossChar)
            {
                BossChar->CurrentSpeed = PunchMoveSpeed; 
            }
        }
    }

    // 2. ジャンプ攻撃 前進制御（XYのみ）
    if (AttackType == EAttackType::JumpAttack && bHasStartedJump)
    {
        FVector Dir = (JumpTargetLocation - Boss->GetActorLocation()).GetSafeNormal2D();
        float Distance = FVector::Dist2D(Boss->GetActorLocation(), JumpTargetLocation);

        if (Distance > 50.f) // 停止距離
        {
            Boss->AddMovementInput(Dir, JumpMoveSpeed * DeltaSeconds);

            // 移動中は歩きアニメーションを再生
            AEnemy_MiddleBoss* BossChar = Cast<AEnemy_MiddleBoss>(Boss);
            if (BossChar)
            {
                BossChar->CurrentSpeed = JumpMoveSpeed;
            }
        }
        else
        {
            bHasStartedJump = false;

            // 停止時は速度をゼロに
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
    if (!Montage || !Boss) return EBTNodeResult::Failed;

    UAnimInstance* AnimInst = Boss->GetMesh()->GetAnimInstance();
    if (!AnimInst) return EBTNodeResult::Failed;

    // Root Motion は無効化して C++で移動
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
        // Notify で衝突判定やダメージ
        break;
    case EAttackType::JumpAttack:
        // Notify で着地判定・範囲ダメージ
        break;
    case EAttackType::Roar:
        // Notify でカメラシェイク・音・バフ
        break;
    default:
        break;
    }
}
