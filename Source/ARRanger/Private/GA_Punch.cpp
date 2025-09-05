#include "GA_Punch.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "ARRangerCharacter.h"
#include "AttackBaseComponent.h"

UGA_Punch::UGA_Punch()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UGA_Punch::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetOwningActorFromActorInfo()))
    {
        Char->GA_PunchInstance = this;
        attackBaseComp = Char->AttackBaseComp;
    }  
    attackBaseComp->SetIsAttacked(false);
    attackBaseComp->SetIsStrongAttacked(false);
    attackBaseComp->SetIsAttractingEnemy(false);

    StartPunch();

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_Punch::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    UE_LOG(LogTemp, Warning, TEXT("Netsu... Atatakana Hikari..."));
    attackBaseComp->SetIsAttacked(false);
    attackBaseComp->SetIsStrongAttacked(false);
    attackBaseComp->SetIsAttractingEnemy(false);

    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    Char->SetIsAttacked(false);
    Char->SetIsStrongAttacked(false);

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Punch::StartPunch()
{
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char || !PunchData.Montage_Normal)
    {
        return;
    }

    attackBaseComp->RotateOwnerToTarget();

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
        // 強攻撃フラグを上げる
        // 引き寄せフラグを下げる
        attackBaseComp->SetIsAttacked(true);
        Char->SetIsAttracted(false);
        Char->SetIsApproachedEnemy(false);

        // パンチを開始
        attackBaseComp->PlayAttackMontage(PunchData);
        return;
    }

    if (EARMagnetismType::Attraction == Char->GetMagnetismType() && Char->GetIsLockedOn() && !Char->GetIsAttracted())
    {
        if (AEnemy* Enemy = Cast<AEnemy>(Char->LockOnComponent->GetLockedOnTarget()))
        {
            // 敵に引き寄せを命令
            Enemy->StartAttraction(Char);
            Char->SetIsAttracted(true);

            // 引き寄せアニメーションを再生
            if (PunchData.Montage_AR && !Char->GetMesh()->GetAnimInstance()->Montage_IsPlaying(PunchData.Montage_AR))
            {
                UE_LOG(LogTemp, Warning, TEXT("Attraction Start!"));
                Char->GetMesh()->GetAnimInstance()->Montage_Play(PunchData.Montage_AR);
            }
            return;
        }
    }

    // モンタージュ未再生 → 1段目から開始
    if (!Anim->Montage_IsPlaying(PunchData.Montage_Normal) && !Char->GetIsAttracted())
    {
        UE_LOG(LogTemp, Warning, TEXT("Combo 1"));
        attackBaseComp->SetIsAttacked(true);
        Char->SetIsAttacked(true);

        Char->ResetComboCount();

        Anim->Montage_Play(PunchData.Montage_Normal);
        Anim->Montage_JumpToSection(GetPunchSectionName(0), PunchData.Montage_Normal);

        // モンタージュ終了通知登録(既存の登録を事前に削除しておく)
        Anim->OnMontageEnded.RemoveDynamic(this, &UGA_Punch::OnAttackMontageEnded);
        Anim->OnMontageEnded.AddDynamic(this, &UGA_Punch::OnAttackMontageEnded);

        return;
    }

    // モンタージュ再生中 → コンボ窓内なら次段にジャンプ
    if (bInComboWindow)
    {
        if (Char->GetComboCount() < MaxCombo - 1)
        {
            attackBaseComp->SetIsAttacked(true);
            Char->SetIsAttacked(true);

            Char->AddComboCount();
            UE_LOG(LogTemp, Warning, TEXT("Current Combo is %d"), Char->GetComboCount() + 1);
            Anim->Montage_JumpToSection(GetPunchSectionName(Char->GetComboCount()), PunchData.Montage_Normal);
        }
    }
    else
    {
        // コンボ窓外でもボタン押したらバッファ
        //bComboQueued = true;
    }
}

FName UGA_Punch::GetPunchSectionName(int32 Index) const
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

void UGA_Punch::ComboWindowStart()
{
    UE_LOG(LogTemp, Warning, TEXT("Combo Start"));
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (Char)
    {
        Char->SetInComboWindow(true);
    }
}

void UGA_Punch::ComboWindowEnd()
{
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (Char)
    {
        Char->SetInComboWindow(false);

        // 次段入力なしならモンタージュ終了
        if (UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance())
        {
            Anim->Montage_Stop(0.15f, PunchData.Montage_Normal);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Combo End"));
}

void UGA_Punch::PunchHitNotify()
{
    attackBaseComp->AttackHit(PunchData);
}

void UGA_Punch::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 最後の段の終了で Ability を終了
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);

    // 状態リセット
    if (AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo()))
    {
        Char->SetIsAttacked(false);
        Char->SetIsStrongAttacked(false);
        Char->ResetComboCount();
    }

    if (attackBaseComp)
    {
        attackBaseComp->SetIsAttacked(false);
        attackBaseComp->SetIsStrongAttacked(false);
        attackBaseComp->SetIsAttractingEnemy(false);
    }
}