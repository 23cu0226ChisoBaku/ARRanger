#include "GA_Attack.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "ARRangerCharacter.h"
#include "Enemy.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

#include "BattleSystem/IBattleSystemInterface.h"
#include "BattleSystem/IARAttackerInterface.h"
#include "BattleSystem/IARAttackable.h"

UGA_Attack::UGA_Attack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
    bIsAttacked = false;
    bIsStrongAttack = false;
    bIsBlowedAwayEnemy = false;
    bIsAttractingEnemy = false;
}

void UGA_Attack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetOwningActorFromActorInfo()))
    {
        Char->GA_AttackInstance = this;
    }

    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (UAnimInstance* AnimInstance = Char->GetMesh()->GetAnimInstance())
    {
        // AnimInstance�Ƀo�C���h
        AnimInstance->OnMontageEnded.AddDynamic(this, &UGA_Attack::OnAttackMontageEnded);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("NO AnimInstance at ActivateAbility!"));
    }

    // �ǂ� AbilitySpec ����������������
    if (Handle == Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo())->PunchHandle)
    {
        StartPunch();
    }
    else if (Handle == Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo())->KickHandle)
    {
        StartKick();
    }
}

void UGA_Attack::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    bIsAttacked = false;
    bIsStrongAttack = false;
    bIsBlowedAwayEnemy = false;
    bIsAttractingEnemy = false;

    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    Char->SetIsAttacked(false);
    Char->SetIsStrongAttacked(false);

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// =====================
// AttackComponent����ڐA
// =====================

void UGA_Attack::StartPunch()
{
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char || !PunchData.Montage_Normal) return;

    RotateOwnerToTarget();

    bool bInComboWindow = Char->GetIsInComboWindow();

    UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
    if (!Anim)
    {
        UE_LOG(LogTemp, Error, TEXT("NO AnimInstance at StartPunch!"));
        return;
    }

    if (Char->GetIsApproachedEnemy())
    {
        UE_LOG(LogTemp, Warning, TEXT("Attraction Punch!"));
        // ���U���t���O���グ��
        // �����񂹃t���O��������
        bIsStrongAttack = true;
        Char->SetIsAttracted(false);
        Char->SetIsApproachedEnemy(false);

        // �p���`���J�n
        PlayAttackMontage(PunchData);
        return;
    }

    if (EARMagnetismType::Attraction == Char->GetMagnetismType() && Char->GetIsLockedOn() && !Char->GetIsAttracted())
    {
        if (AEnemy* Enemy = Cast<AEnemy>(Char->LockOnComponent->GetLockedOnTarget()))
        {
            // �G�Ɉ����񂹂𖽗�
            Enemy->StartAttraction(Char); 
            Char->SetIsAttracted(true);

            // �����񂹃A�j���[�V�������Đ�
            if (PunchData.Montage_AR && !Char->GetMesh()->GetAnimInstance()->Montage_IsPlaying(PunchData.Montage_AR))
            {
                UE_LOG(LogTemp, Warning, TEXT("Attraction Start!"));
                Char->GetMesh()->GetAnimInstance()->Montage_Play(PunchData.Montage_AR);
            }
            return;
        }
    }
    
    // �����^�[�W�����Đ� �� 1�i�ڂ���J�n
    if (!Anim->Montage_IsPlaying(PunchData.Montage_Normal) && !Char->GetIsAttracted())
    {
        UE_LOG(LogTemp, Warning, TEXT("Combo 1"));
        bIsAttacked = true;
        Char->SetIsAttacked(true);

        Char->ResetComboCount();
        bComboQueued = false;
        bNextScheduled = false;

        Anim->Montage_Play(PunchData.Montage_Normal);
        Anim->Montage_JumpToSection(GetPunchSectionName(0), PunchData.Montage_Normal);

        return;
    }

    // �����^�[�W���Đ��� �� �R���{�����Ȃ玟�i�ɃW�����v
    if (bInComboWindow)
    {
        if (Char->GetComboCount() < MaxCombo - 1)
        {
            bIsAttacked = true;
            Char->SetIsAttacked(true);

            Char->AddComboCount();
            UE_LOG(LogTemp, Warning, TEXT("Current Combo is %d"), Char->GetComboCount() + 1);
            Anim->Montage_JumpToSection(GetPunchSectionName(Char->GetComboCount()), PunchData.Montage_Normal);
            bNextScheduled = true;
            bComboQueued = false;
        }
    }
    else
    {
        // �R���{���O�ł��{�^����������o�b�t�@
        //bComboQueued = true;
    }
}

void UGA_Attack::PunchHitNotify()
{
    AttackHit(PunchData);
}

void UGA_Attack::StartKick()
{
    if (bIsAttacked) return;

    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char) return;

    RotateOwnerToTarget();

    if (Char->GetMagnetismType() == EARMagnetismType::Repulsion &&
        Char->LockOnComponent->GetIsLockedOn())
    {
        bIsBlowedAwayEnemy = true;
        bIsStrongAttack = true;
        Char->SetIsStrongAttacked(true);

        PlayAttackMontage(KickData);
        return;
    }

    PlayAttackMontage(KickData);
}

void UGA_Attack::KickHitNotify()
{
    AttackHit(KickData);
}

void UGA_Attack::RotateOwnerToTarget()
{
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char || !Char->LockOnComponent || !Char->LockOnComponent->GetIsLockedOn()) return;

    AActor* Target = Char->LockOnComponent->GetLockedOnTarget();
    if (!Target) return;

    FVector Dir = Target->GetActorLocation() - Char->GetActorLocation();
    Dir.Z = 0.f;
    Dir.Normalize();

    FRotator TargetRot = Dir.Rotation();
    Char->SetActorRotation(TargetRot);
}

void UGA_Attack::PlayAttackMontage(const FAttackData& Attack)
{
    // �v���C���[�����Ȃ����AMontage���ݒ肳��Ă��Ȃ���Ώ������Ȃ�
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char || !Attack.Montage_Normal)
    {
        return;
    }

    // AnimInstance���Ȃ���Ώ������Ȃ�
    UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
    if (!Anim)
    {
        return;
    }

    // ���ɓ��������^�[�W���Đ����Ȃ炱���ŉ������Ȃ�(�p���`��StartPunch���ʓ|������)
    if (Anim->Montage_IsPlaying(Attack.Montage_Normal))
    {
        return;
    }
        
    bIsAttacked = true;
    Char->SetIsAttacked(true);

    if (bIsStrongAttack && Attack.Montage_Strong)
    {
        Anim->Montage_Play(Attack.Montage_Strong);
    }
    else
    {
        Anim->Montage_Play(Attack.Montage_Normal);
    }
}

void UGA_Attack::AttackHit(const FAttackData& Attack)
{
  AActor* avatarActor = GetAvatarActorFromActorInfo();
  if (avatarActor == nullptr)
  {
    return;
  }

  const float AttackOffset = 100.0f;
  const FVector Origin = avatarActor->GetActorLocation() + avatarActor->GetActorForwardVector() * AttackOffset;
  TArray<AActor*> HitActors{};

  const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
                      avatarActor,
                      Origin,
                      Attack.HitRadius,
                      { UEngineTypes::ConvertToObjectType(ECC_Pawn), UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) },
                      nullptr,
                      { avatarActor },
                      HitActors
                    );
  if (!bHit)
  {
    return;
  }

  for (AActor* HitActor : HitActors)
  {
    // Edited by MAI
    if (HitActor->GetClass()->ImplementsInterface(UARAttackable::StaticClass()))
    {
      IARAttackable* attackable = ::Cast<IARAttackable>(HitActor);

      // NOTE Call this inside AARRangerCharacter
      // // NotifyHandler�͂�����ł͐G�ꂸ�A�v���C���[���ɔC����
      // Char->OnAttackHitNotify();

      // 攻撃処理
      if (avatarActor->GetClass()->ImplementsInterface(UARAttackerInterface::StaticClass()))
      {
        FARAttackParameters attackParam{};
        attackParam.Instigator = avatarActor;
        attackParam.bUseAttackerActor = false;
        
        attackParam.Damage = Attack.Damage;
        if (bIsStrongAttack)
        {
          attackParam.Damage += Attack.DamageModifier;
        }

        FVector LaunchDir{EForceInit::ForceInitToZero};
        if (bIsBlowedAwayEnemy)
        {
          const FVector ImpulseDir_Norm = ((HitActor->GetActorLocation() - avatarActor->GetActorLocation()).GetSafeNormal() + FVector{0.0, 0.0, 0.5}).GetSafeNormal();
          LaunchDir = avatarActor->GetActorForwardVector() + FVector{0.0, 0.0, 0.2};
          LaunchDir.Normalize();
          LaunchDir += ImpulseDir_Norm;
          LaunchDir.Normalize();
        }
        attackParam.LaunchDirection = LaunchDir;

        // ターゲットを攻撃する
        (void)attackable->AttackTarget(::Cast<IARAttackerInterface>(GetAvatarActorFromActorInfo()), attackParam);

        // Hitエフェクトを生成
        const FVector SpawnLocation = HitActor->GetActorLocation();
        const FRotator SpawnRotation = FRotator::ZeroRotator;
        GetWorld()->SpawnActor<AActor>(HitEffectActor, SpawnLocation, SpawnRotation);
      }
    }
  }
}

void UGA_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Warning, TEXT("Montage End"));
    bIsAttacked = false;
    bIsStrongAttack = false;
    bIsBlowedAwayEnemy = false;
    bIsAttractingEnemy = false;

    bComboQueued = false;
    bNextScheduled = false;

    if (AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo()))
    {
        Char->SetIsAttacked(false);
        Char->SetIsStrongAttacked(false);
        Char->ResetComboCount();
    }
}

void UGA_Attack::ComboWindowStart()
{
    UE_LOG(LogTemp, Warning, TEXT("Combo Start"));
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (Char)
    {
        Char->SetInComboWindow(true);
    }
}

void UGA_Attack::ComboWindowEnd()
{
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (Char)
    {
        Char->SetInComboWindow(false);

        // ���i���͂Ȃ��Ȃ烂���^�[�W���I��
        if (UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance())
        {
            Anim->Montage_Stop(0.05f, PunchData.Montage_Normal);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Combo End"));
}

FName UGA_Attack::GetPunchSectionName(int32 Index) const
{
    // �Z�N�V���������擾
    int32 Clamped = FMath::Clamp(Index, 0, MaxCombo - 1);
    switch (Clamped)
    {
    case 0:  return FName(TEXT("Punch1"));
    case 1:  return FName(TEXT("Punch2"));
    case 2:  return FName(TEXT("Punch3"));
    default:  return FName(TEXT(""));
    }
}