#include "GA_Attack.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "ARRangerCharacter.h"
#include "Enemy.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

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
        // AnimInstanceにバインド
        AnimInstance->OnMontageEnded.AddDynamic(this, &UGA_Attack::OnAttackMontageEnded);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("NO AnimInstance at ActivateAbility!"));
    }

    // どの AbilitySpec が発動したか判定
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
    UE_LOG(LogTemp, Warning, TEXT("Korega Konoyono Hate..."));
    bIsAttacked = false;
    bIsStrongAttack = false;
    bIsBlowedAwayEnemy = false;
    bIsAttractingEnemy = false;
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    Char->SetIsAttacked(false);
    Char->SetIsStrongAttacked(false);
    Char->SetIsAttracted(false);

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// =====================
// AttackComponentから移植
// =====================
void UGA_Attack::StartPunch()
{
    UE_LOG(LogTemp, Warning, TEXT("Punch Start"));
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char || !PunchData.Montage_Normal) return;

    RotateOwnerToTarget();

    bool bInComboWindow = Char->GetIsInComboWindow();
    UE_LOG(LogTemp, Warning, TEXT("Combo OK? %s"), bInComboWindow ? TEXT("true") : TEXT("false"));


    UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
    if (!Anim)
    {
        UE_LOG(LogTemp, Error, TEXT("NO AnimInstance at StartPunch!"));
        return;
    }
        
    int32 ComboCount = Char->GetComboCount();
    UE_LOG(LogTemp, Warning, TEXT("Combo Count is %d"), ComboCount + 1);

    // モンタージュ未再生 → 1段目から開始
    if (!Anim->Montage_IsPlaying(PunchData.Montage_Normal))
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

    // モンタージュ再生中 → コンボ窓内なら次段にジャンプ
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
        // コンボ窓外でもボタン押したらバッファ
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
    // プレイヤーがいないか、Montageが設定されていなければ処理しない
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char || !Attack.Montage_Normal)
    {
        return;
    }

    // AnimInstanceがなければ処理しない
    UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
    if (!Anim)
    {
        return;
    }

    // 既に同じモンタージュ再生中ならここで何もしない(パンチはStartPunchが面倒を見る)
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
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char)
    {
        return;
    }

    FVector Origin = Char->GetActorLocation() + Char->GetActorForwardVector() * 100.f;
    TArray<AActor*> HitActors;

    // 当たり判定を作成
    bool bHit = UKismetSystemLibrary::SphereOverlapActors(
        Char,
        Origin,
        Attack.HitRadius,
        { UEngineTypes::ConvertToObjectType(ECC_Pawn), UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) },
        nullptr,
        { Char },
        HitActors
    );

    if (!bHit)
    {
        return;
    }

    for (AActor* HitActor : HitActors)
    {
        if (HitActor->ActorHasTag(Attack.TargetTag))
        {
            AEnemy* Enemy = Cast<AEnemy>(HitActor);

            if (Enemy && !Enemy->isDead)
            {
                // NotifyHandlerはこちらでは触れず、プレイヤー側に任せる
                Char->OnAttackHitNotify();

                // ヒットエフェクト用のアクターをSpawn
                FVector SpawnLocation = Enemy->GetActorLocation();
                FRotator SpawnRotation = FRotator::ZeroRotator;

                GetWorld()->SpawnActor<AActor>(HitEffectActor, SpawnLocation, SpawnRotation);

                FVector LaunchDir = Char->GetActorForwardVector() + FVector(0, 0, 0.2f);
                LaunchDir.Normalize();

                // ダメージを与える(強攻撃ならダメージを上乗せ)
                if (bIsStrongAttack)
                {
                    const bool bWillBeKilled = (Enemy->currentHP - (Attack.Damage + Attack.DamageModifier) <= 0);
                    Enemy->ReceiveDamage(Attack.Damage + Attack.DamageModifier, LaunchDir, bWillBeKilled);

                    // 斥力キック時は敵を吹っ飛ばす
                    if (bIsBlowedAwayEnemy)
                    {
                        UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Enemy->GetRootComponent());
                        if (PrimComp && PrimComp->IsSimulatingPhysics())
                        {
                            FVector ImpulseDir = (Enemy->GetActorLocation() - Char->GetActorLocation()).GetSafeNormal();
                            ImpulseDir.Z += 0.5f;
                            ImpulseDir.Normalize();

                            float ImpulseStrength = 1500.f;

                            PrimComp->AddImpulse(ImpulseDir * ImpulseStrength, NAME_None, true);
                        }
                    }
                }
                else
                {
                    const bool bWillBeKilled = (Enemy->currentHP - Attack.Damage <= 0);
                    Enemy->ReceiveDamage(Attack.Damage, LaunchDir, bWillBeKilled);
                }
            }
        }
    }
}

void UGA_Attack::ScheduleNextPunch()
{
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char || !PunchData.Montage_Normal) return;

    int32 ComboCount = Char->GetComboCount();
    if (ComboCount >= MaxCombo - 1) return;  

    UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
    if (!Anim) return;

    const FName CurSection = GetPunchSectionName(ComboCount);
    const FName NextSection = GetPunchSectionName(ComboCount + 1);

    Anim->Montage_SetNextSection(CurSection, NextSection, PunchData.Montage_Normal);

    bNextScheduled = true;
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
        Char->SetIsAttracted(false);
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

        // 次段入力なしならモンタージュ終了
        if (UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance())
        {
            Anim->Montage_Stop(0.05f, PunchData.Montage_Normal);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Combo End"));
}

FName UGA_Attack::GetPunchSectionName(int32 Index) const
{
    // セクション名を取得
    int32 Clamped = FMath::Clamp(Index, 0, MaxCombo - 1);
    switch (Clamped)
    {
    case 0:  return FName(TEXT("Punch1"));
    case 1:  return FName(TEXT("Punch2"));
    case 2:  return FName(TEXT("Punch3"));
    default:  return FName(TEXT(""));
    }
}