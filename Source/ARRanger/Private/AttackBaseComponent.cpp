#include "AttackBaseComponent.h"

#include "ARRangerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

UAttackBaseComponent::UAttackBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

    // プレイヤーとそのコントローラーを取得
    ownerPawn = Cast<AARRangerCharacter>(GetOwner());
    ownerController = ownerPawn ? Cast<APlayerController>(ownerPawn->GetController()) : nullptr;
}

void UAttackBaseComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttackBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttackBaseComponent::RotateOwnerToTarget()
{
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(ownerPawn);
    if (!Char || !Char->LockOnComponent || !Char->LockOnComponent->GetIsLockedOn())
    {
        return;
    }

    AActor* Target = Char->LockOnComponent->GetLockedOnTarget();
    if (!Target)
    {
        return;
    }
        
    FVector Dir = Target->GetActorLocation() - Char->GetActorLocation();
    Dir.Z = 0.f;
    Dir.Normalize();

    FRotator TargetRot = Dir.Rotation();
    Char->SetActorRotation(TargetRot);
}

void UAttackBaseComponent::PlayAttackMontage(const FAttackData& Attack)
{
    // プレイヤーがいないか、Montageが設定されていなければ処理しない
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(ownerPawn);
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

    if (bIsStrongAttacked && Attack.Montage_Strong)
    {
        Anim->Montage_Play(Attack.Montage_Strong);
    }
    else
    {
        Anim->Montage_Play(Attack.Montage_Normal);
    }
}

void UAttackBaseComponent::AttackHit(const FAttackData& Attack)
{
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(ownerPawn);
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
                if (bIsStrongAttacked)
                {
                    const bool bWillBeKilled = (Enemy->currentHP - (Attack.Damage + Attack.DamageModifier) <= 0);
                    Enemy->ReceiveDamage(bIsStrongAttacked, Attack.Damage + Attack.DamageModifier, LaunchDir, bWillBeKilled);

                    // 斥力キック時は敵を吹っ飛ばす
                    if (bIsBlowedAwayEnemy)
                    {
                        UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Enemy->GetRootComponent());
                        if (PrimComp && PrimComp->IsSimulatingPhysics())
                        {
                            FVector ImpulseDir = (Enemy->GetActorLocation() - Char->GetActorLocation()).GetSafeNormal();
                            ImpulseDir.Z += 0.5f;
                            ImpulseDir.Normalize();

                            float ImpulseStrength = 1300.f;

                            PrimComp->AddImpulse(ImpulseDir * ImpulseStrength, NAME_None, true);
                            return;
                        }
                    }

                    // 引力パンチ時は斥力キック時よりも控えめに吹っ飛ばす
                    UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Enemy->GetRootComponent());
                    FVector ImpulseDir = (Enemy->GetActorLocation() - Char->GetActorLocation()).GetSafeNormal();
                    ImpulseDir.Z += 0.5f;
                    ImpulseDir.Normalize();

                    float ImpulseStrength = 1000.f;

                    PrimComp->AddImpulse(ImpulseDir * ImpulseStrength, NAME_None, true);
                }
                else
                {
                    const bool bWillBeKilled = (Enemy->currentHP - Attack.Damage <= 0);
                    Enemy->ReceiveDamage(bIsStrongAttacked, Attack.Damage, LaunchDir, bWillBeKilled);
                }
            }
        }
    }
}

void UAttackBaseComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Warning, TEXT("Montage End"));
    bIsAttacked = false;
    bIsStrongAttacked = false;
    bIsBlowedAwayEnemy = false;
    bIsAttractingEnemy = false;

    if (AARRangerCharacter* Char = Cast<AARRangerCharacter>(ownerPawn))
    {
        Char->SetIsAttacked(false);
        Char->SetIsStrongAttacked(false);
        Char->ResetComboCount();
    }
}