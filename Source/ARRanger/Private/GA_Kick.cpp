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
        Char->GA_KickInstance = this;
        attackBaseComp = Char->AttackBaseComp;
    }
    attackBaseComp->SetIsAttacked(false);
    attackBaseComp->SetIsStrongAttacked(false);
    attackBaseComp->SetIsAttractingEnemy(false);

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
    UE_LOG(LogTemp, Warning, TEXT("Netsu... Atatakana Hikari..."));
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

    // モンタージュ終了通知登録(既存の登録を事前に削除しておく)
    UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
    Anim->OnMontageEnded.RemoveDynamic(this, &UGA_Kick::OnAttackMontageEnded);
    Anim->OnMontageEnded.AddDynamic(this, &UGA_Kick::OnAttackMontageEnded);

    if (Char->GetMagnetismType() == EARMagnetismType::Repulsion &&
        Char->LockOnComponent->GetIsLockedOn())
    {
        attackBaseComp->SetIsBlowedAwayEnemy(true);
        attackBaseComp->SetIsStrongAttacked(true);
        Char->SetIsStrongAttacked(true);

        attackBaseComp->PlayAttackMontage(KickData);
        return;
    }

    attackBaseComp->PlayAttackMontage(KickData);
}

void UGA_Kick::KickHitNotify()
{
    attackBaseComp->AttackHit(KickData);
}

void UGA_Kick::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
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
        attackBaseComp->SetIsBlowedAwayEnemy(false);
    }
}