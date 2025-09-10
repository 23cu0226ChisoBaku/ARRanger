#include "GA_Kick.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "ARRangerCharacter.h"
#include "AttackBaseComponent.h"

UGA_Kick::UGA_Kick()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UGA_Kick::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetOwningActorFromActorInfo()))
    {
        attackBaseComp = Char->AttackBaseComp;
    }
    attackBaseComp->SetIsAttacked(false);
    attackBaseComp->SetIsStrongAttacked(false);
    attackBaseComp->SetIsAttractingEnemy(false);

    UE_LOG(LogTemp, Warning, TEXT("ikuyo--"));
    StartKick();

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_Kick::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    UE_LOG(LogTemp, Warning, TEXT("Korega Konoyono Hate..."));
    attackBaseComp->SetIsAttacked(false);
    attackBaseComp->SetIsStrongAttacked(false);
    attackBaseComp->SetIsBlowedAwayEnemy(false);

    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    Char->SetIsAttacked(false);
    Char->SetIsStrongAttacked(false);

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Kick::StartKick()
{
    // 攻撃中は処理しない
    if (attackBaseComp->GetIsAttacked())
    {
        return;
    }

    // プレイヤーがいなければ処理しない
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char)
    {
        return;
    }

    attackBaseComp->RotateOwnerToTarget();

    attackBaseComp->SetIsStrongAttacked(true);
    Char->SetIsStrongAttacked(true);

    chargeStartTime = GetWorld()->GetTimeSeconds();

    // チャージモーション再生
    if (KickData.Montage_AR)
    {
        UE_LOG(LogTemp, Warning, TEXT("Charrrrrge"));
        UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
        Anim->Montage_Play(KickData.Montage_AR);

        Anim->OnMontageEnded.RemoveDynamic(this, &UGA_Kick::OnAttackMontageEnded);
        Anim->OnMontageEnded.AddDynamic(this, &UGA_Kick::OnAttackMontageEnded);
    }
}

void UGA_Kick::KickHitNotify()
{
    attackBaseComp->AttackHit(KickData);
}

void UGA_Kick::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Warning, TEXT("Montage End"));
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
        attackBaseComp->SetIsBlowedAwayEnemy(false);
    }
}

void UGA_Kick::InputReleased()
{
    UE_LOG(LogTemp, Warning, TEXT("Yobareteru?"));
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char)
    {
        return;
    }

    // チャージ時間計算
    chargeDuration = GetWorld()->GetTimeSeconds() - chargeStartTime;

    // 段階判定
    if (chargeDuration < 1.0f)
    {
        chargeLevel = 1;
    }  
    else if (chargeDuration < 2.0f)
    {
        chargeLevel = 2;
    } 
    else
    {
        chargeLevel = 3;
    }
        
    // チャージモーション停止
    if (KickData.Montage_AR)
    {
        Char->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, KickData.Montage_AR);
    }

    // ダメージ・ノックバック設定
    switch (chargeLevel)
    {
    case 1: 
        KickData.Damage = 45; 
        //KickData.Knockback = 200.f; 
        break;
    case 2: 
        KickData.Damage = 65;
        //KickData.Knockback = 400.f; 
        break;
    case 3: 
        KickData.Damage = 100;
        //KickData.Knockback = 800.f; 
        break;
    }

    // キックアニメ再生
    if (KickData.Montage_Strong)
    {
        Char->GetMesh()->GetAnimInstance()->Montage_Play(KickData.Montage_Strong, 1.0f);
    }
}