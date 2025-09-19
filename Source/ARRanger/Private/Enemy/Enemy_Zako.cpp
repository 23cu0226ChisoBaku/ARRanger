
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

    if (OnDead.IsBound())
    {
      OnDead.Broadcast(this);
    }

    // TODO Bad thing
    if (AInsekiGameMode* GM = Cast<AInsekiGameMode>(UGameplayStatics::GetGameMode(this)))
    {
      GM->OnEnemyKilled(this);
    }

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetAllBodiesSimulatePhysics(true);
    GetMesh()->SetAllBodiesPhysicsBlendWeight(1.0f);
    GetMesh()->bBlendPhysics = true;

    // 死亡時は大きく吹っ飛ばす
    const FVector DeathImpulse = LaunchDirection * 5000.0f;
    GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

    SetLifeSpan(3.0f);
  }
  else
  {
    // 生存時は前方ベクトルの逆方向にノックバック
    const FVector KnockbackDir = LaunchDirection * 1000.f;
    LaunchCharacter(KnockbackDir, true, true);
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

  K2_ReceiveDamage(DamageAmount, LaunchDirection, IsDead());
}

void AEnemy_Zako::Zako_PerformAttack()
{
  if (isDead)
  {
    return;
  }

  K2_PerformAttack();
}

bool AEnemy_Zako::IsDead()
{
  return (currentHP <= 0) && isDead;
}

// ==== IARAttackable 実装 ====
bool AEnemy_Zako::CanAttack()
{
  return !IsDead();
}

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