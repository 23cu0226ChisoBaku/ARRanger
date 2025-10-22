
#include "Enemy/Enemy_Zako.h"
#include "Enemy/EnemyAnimInstance.h"
#include "InsekiGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/ZakoAIController.h"
#include "Components/CapsuleComponent.h"
#include "Character/ARHealthComponent.h"

AEnemy_Zako::AEnemy_Zako()
  : isDead{false}
{
  AIControllerClass = AZakoAIController::StaticClass();
  AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

  HealthComponent = CreateDefaultSubobject<UARHealthComponent>(TEXT("HealthComponent"));
  HealthComponent->OnDeadEventStarted.AddDynamic(this, &ThisClass::EnemyDeadStarted);
  HealthComponent->OnDeadEventFinished.AddDynamic(this, &ThisClass::EnemyDeadFinished);
  HealthComponent->OnHealthChanged.AddDynamic(this, &ThisClass::OnEnemyHealthChanged);

}

void AEnemy_Zako::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  // Destroy AIController
  if (AController* controller = GetController())
  {
    controller->UnPossess();
    controller->Destroy();
  }

  if (StartDeadTimer.IsValid())
  {
    GetWorld()->GetTimerManager().ClearTimer(StartDeadTimer);
  }

}

void AEnemy_Zako::SetIsChasing(bool bChasing)
{
  if (UEnemyAnimInstance* Anim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance()))
  {
    Anim->bIsChasing = bChasing;
  }
}

void AEnemy_Zako::ReceiveDamage(AActor* InInstigator, float DamageAmount)
{
  if (IsDead())
  {
    return;
  }

  if (HealthComponent != nullptr)
  {
    HealthComponent->HandleHealthChange(InInstigator, -DamageAmount);

    if (IsDead())
    {
      HealthComponent->StartDead();
    }
  }
}

void AEnemy_Zako::ReceiveLaunch(const FVector& LaunchDirection)
{
  if (bDeadLaunchHandled)
  {
    return;
  }

  if (IsDead())
  {
    bDeadLaunchHandled = true;
    // 死亡時は大きく吹っ飛ばす
    const FVector DeathImpulse = LaunchDirection * 5000.0f;
    GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
  }
  else
  {
    const FVector KnockbackDir = LaunchDirection * 1000.f;
    LaunchCharacter(KnockbackDir, true, true);
  }

}

void AEnemy_Zako::Zako_PerformAttack()
{
  if (IsDead())
  {
    return;
  }

  K2_PerformAttack();
}

bool AEnemy_Zako::IsDead() const
{
  return isDead;
}

// ==== IARAttackable 実装 ====
bool AEnemy_Zako::CanAttack()
{
  return !IsDead();
}

void AEnemy_Zako::OnPreAttacked(const FARAttackParameters& InAttackParams,ARRanger::Battle::FARAttackResult& OutAttackResult)
{
  if (IsDead() || ((InAttackParams.Instigator != nullptr) && InAttackParams.Instigator->IsA(StaticClass())))
  {
    OutAttackResult.Result = ARRanger::Battle::EARAttackResult::Inmune;
    return;
  }

  OutAttackResult.Result = ARRanger::Battle::EARAttackResult::Success;
}

void AEnemy_Zako::OnDamaged(const ARRanger::Battle::FARDamageResult& InDamageResult)
{
  //既存の ReceiveDamage を利用
  ReceiveDamage(InDamageResult.Instigator, InDamageResult.FinalDamage);

  ReceiveLaunch(InDamageResult.FinalLaunchDirection);

  K2_ReceiveAttack(InDamageResult.FinalDamage, InDamageResult.FinalLaunchDirection, InDamageResult.ImpactLocation, IsDead());
}

void AEnemy_Zako::OnPostAttacked(const FARAttackParameters& InAttackParams)
{
  //ヒットエフェクトやSEをここで再生
}

//ISpecialAttractInterface functions Start
//brief 引力必殺技が始まった時の通知
void AEnemy_Zako::OnStartSpecialAttractNotify()
{
  if (IsDead())
  {
    return;
  }
  
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
  if (IsDead())
  {
    return;
  }

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

void AEnemy_Zako::EnemyDeadStarted(AActor* OwningActor)
{
  GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
  GetMesh()->SetSimulatePhysics(true);
  GetMesh()->SetAllBodiesSimulatePhysics(true);
  GetMesh()->SetAllBodiesPhysicsBlendWeight(1.0f);
  GetMesh()->bBlendPhysics = true;

  // Spawn dead start effect
  PerformDeadStartEffect();

  auto deadFinishHandler = [this]
  {
    if (HealthComponent != nullptr)
    {
      HealthComponent->FinishDead();
    }
    else
    {
      EnemyDeadFinished(this);
    }
  };

  GetWorld()->GetTimerManager().SetTimer(StartDeadTimer, deadFinishHandler, 3.0f, false);

  OnEnemyDeadStarted.Broadcast(OwningActor);
}

void AEnemy_Zako::EnemyDeadFinished(AActor* OwningActor)
{
  SetLifeSpan(0.1f);

  OnEnemyDeadEnded.Broadcast(OwningActor);
}

void AEnemy_Zako::OnEnemyHealthChanged(UARHealthComponent* InHealthComponent, AActor* InInstigator, float PreviousHealth, float CurrentHealth)
{
  if (IsDead())
  {
    return;
  }

  if (CurrentHealth <= 0.f)
  {
    isDead = true;
  }
}

void AEnemy_Zako::PerformDeadStartEffect()
{
  const float hitStopEffectMultiplier = 0.3f;
  UGameplayStatics::SetGlobalTimeDilation(this, hitStopEffectMultiplier);
  FTimerHandle Dummy{};
  GetWorldTimerManager().SetTimer(Dummy, 
  [this]()
  {
    UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
  }, 0.03f, false);
}

void AEnemy_Zako::PerformDeadEndEffect()
{
  // TODO Try to do something?
}

void AEnemy_Zako::Destroyed()
{
  PerformDeadEndEffect();

  Super::Destroyed();
} 