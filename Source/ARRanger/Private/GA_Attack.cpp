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
        UE_LOG(LogTemp, Warning, TEXT("AnimInstance found! Registering OnMontageEnded"));
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
    UE_LOG(LogTemp, Warning, TEXT("Punch Start!"));
    if (bIsAttacked) return;

    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char) return;

    // ロックオン向きに回転
    RotateOwnerToTarget();

    // 引力状態で敵をロックオンしていれば引き寄せ開始
    if (Char->GetMagnetismType() == EARMagnetismType::Attraction &&
        Char->LockOnComponent->GetIsLockedOn())
    {
        bIsAttractingEnemy = true;
        bIsStrongAttack = true;
        Char->SetIsStrongAttacked(true);
        Char->SetIsAttracted(true);

        if (PunchData.Montage_AR)
        {
            Char->GetMesh()->GetAnimInstance()->Montage_Play(PunchData.Montage_AR);
        }
        return;
    }

    bIsStrongAttack = false;
    PlayAttackMontage(PunchData);
}

void UGA_Attack::PunchHitNotify()
{
    AttackHit(PunchData);
}

void UGA_Attack::StartKick()
{
    UE_LOG(LogTemp, Warning, TEXT("Kick Start!"));
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
    UE_LOG(LogTemp, Warning, TEXT("Rotate To Target"));
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
    // プレイヤーが取得できなければ処理しない
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    if (!Char)
    {
        return;
    }
    
    // アニメーションがない、または攻撃中は処理しない 
    if (!Attack.Montage_Normal || !Attack.Montage_Strong || bIsAttacked) 
    { 
        return; 
    } 

    UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance(); 
    // アニメ再生中は処理しない 
    if (!Anim || Anim->Montage_IsPlaying(Attack.Montage_Normal) || Anim->Montage_IsPlaying(Attack.Montage_Strong)) 
    { 
        return;
    }

    bIsAttacked = true;
    Char->SetIsAttacked(true);

    UE_LOG(LogTemp, Warning, TEXT("Anim Start"));
    if (bIsStrongAttack)
        Anim->Montage_Play(Attack.Montage_Strong);
    else
        Anim->Montage_Play(Attack.Montage_Normal);
}

void UGA_Attack::AttackHit(const FAttackData& Attack)
{
    UE_LOG(LogTemp, Warning, TEXT("Hit!"));
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

void UGA_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Warning, TEXT("Anim End"));
    bIsAttacked = false;
    bIsStrongAttack = false;
    bIsBlowedAwayEnemy = false;
    bIsAttractingEnemy = false;

    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    Char->SetIsAttacked(false);
    Char->SetIsStrongAttacked(false);
    Char->SetIsAttracted(false);
}