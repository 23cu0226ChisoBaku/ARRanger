
#include "Enemy/Enemy_Zako.h"
#include "Enemy/EnemyAnimInstance.h"
#include "InsekiGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/ZakoAIController.h"
#include "Components/CapsuleComponent.h"

AEnemy_Zako::AEnemy_Zako()
    : maxHP(100)
    , currentHP(maxHP)
    , isDead(false)
{
    AIControllerClass = AZakoAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemy_Zako::SetIsChasing(bool bChasing)
{
    if (UEnemyAnimInstance* Anim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        Anim->bIsChasing = bChasing;
    }
}

void AEnemy_Zako::ReceiveDamage(int DamageAmount, FVector LaunchDirection, bool bEnableHitStop)
{
    currentHP -= DamageAmount;

    if (currentHP <= 0 && !isDead)
    {
        isDead = true;

        if (AInsekiGameMode* GM = Cast<AInsekiGameMode>(UGameplayStatics::GetGameMode(this)))
        {
            GM->OnEnemyKilled();
        }

        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
        GetMesh()->SetSimulatePhysics(true);
        GetMesh()->SetAllBodiesSimulatePhysics(true);
        GetMesh()->SetAllBodiesPhysicsBlendWeight(1.0f);
        GetMesh()->bBlendPhysics = true;

        // 死亡時は大きく吹っ飛ばす
        FVector DeathImpulse = -GetActorForwardVector() * 5000.0f;
        GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

        SetLifeSpan(3.0f);
    }
    else
    {
        // 生存時は前方ベクトルの逆方向にノックバック
        FVector KnockbackDir = -GetActorForwardVector();
        LaunchCharacter(KnockbackDir * 1000.f, true, true);
    }

    if (bEnableHitStop)
    {
        UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, []()
            {
                UGameplayStatics::SetGlobalTimeDilation(GWorld, 1.0f);
            }, 0.03f, false);
    }
}

void AEnemy_Zako::Zako_PerformAttack()
{
    
    if (isDead) return;

    UE_LOG(LogTemp, Log, TEXT("Enemy_Zako: PerforAttack executed."));

    //攻撃モンタージュ再生
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        if (AttackMontage)
        {
            AnimInstance->Montage_Play(AttackMontage);
        }
    }

    //攻撃判定
    /*FVector AttackCenter = GetActorLocation() + GetActorForwardVector() * 100.f;
    float AttackRadius = 150.f;

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->OverlapMultiByChannel(
        Overlaps,
        AttackCenter,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(AttackRadius),
        Params
    );

    if (bHit)
    {
        for (auto& Result : Overlaps)
        {
            if (AActor* HitActor = Result.GetActor())
            {
                UE_LOG(LogTemp, Log, TEXT("Enemy_Zako hit: %s"), *HitActor->GetName());

                // IARAttackable を持っていれば攻撃イベントを送る
                if (HitActor->GetClass()->ImplementsInterface(UIARAttackable::StaticClass()))
                {
                    FARAttackParameters AttackParams;
                    AttackParams.Attacker = this;
                    AttackParams.Damage = 10; // ← 適宜調整
                    AttackParams.LaunchDirection = GetActorForwardVector();

                    ARRanger::Battle::FARAttackResult ResultData;
                    IARAttackable::Execute_OnPreAttacked(HitActor, AttackParams, ResultData);
                    IARAttackable::Execute_OnPostAttacked(HitActor, AttackParams);
                }
            }
        }
    }*/
}


// ==== IARAttackable 実装 ====
void AEnemy_Zako::OnPreAttacked(const FARAttackParameters& InAttackParams,ARRanger::Battle::FARAttackResult& OutAttackResult)
{
    if (isDead)
    {
        OutAttackResult.Result = ARRanger::Battle::EARAttackResult::Inmune;
        return;
    }
    OutAttackResult.Result = ARRanger::Battle::EARAttackResult::Success;
}

void AEnemy_Zako::OnDamaged(const ARRanger::Battle::FARDamageResult& InDamageResult)
{
    //既存の ReceiveDamage を利用
    ReceiveDamage(static_cast<int32>(InDamageResult.FinalDamage),
        InDamageResult.FinalLaunchDirection,
            (currentHP - InDamageResult.FinalDamage <= 0));
}

void AEnemy_Zako::OnPostAttacked(const FARAttackParameters& InAttackParams)
{
    //ヒットエフェクトやSEをここで再生

}

//ISpecialAttractInterface functions Start
//brief 引力必殺技が始まった時の通知
void AEnemy_Zako::OnStartSpecialAttractNotify()
{
    /*重力をゼロにする*/
    if (ACharacter* character = Cast<ACharacter>(this))
    {
        character->GetCharacterMovement()->GravityScale = 0.0f;
    }
}

//引力必殺技の中間通知
void AEnemy_Zako::OnUpdateSpecialAttractNotify(float elapsed)
{
    ;
}

//brief 引力必殺技の終了通知
void AEnemy_Zako::OnEndSpecialAttractNotify()
{
    /*重力を元に戻す*/
    if (ACharacter* character = Cast<ACharacter>(this))
    {
        character->GetCharacterMovement()->GravityScale = 1.0f;
    }
}

void AEnemy_Zako::StartAttraction(AActor* Target)
{
    attractionTarget = Target;
    bIsAttracted = true;
}

void AEnemy_Zako::StopAttraction()
{
    bIsAttracted = false;
    attractionTarget = nullptr;
}