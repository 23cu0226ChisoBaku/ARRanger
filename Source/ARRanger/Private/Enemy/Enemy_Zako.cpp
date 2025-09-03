
#include "Enemy/Enemy_Zako.h"
#include "Enemy/EnemyAnimInstance.h"
#include "InsekiGameMode.h"
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

        GetMesh()->AddImpulse(LaunchDirection * 5000.0f, NAME_None, true);
        SetLifeSpan(3.0f);
    }
    else
    {
        LaunchCharacter(LaunchDirection * 600.f, true, true);
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
