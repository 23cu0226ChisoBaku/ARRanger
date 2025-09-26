// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy_MiddleBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Enemy/EnemyAttackTypes.h"

class FAttackTask_ApproachTarget : public AEnemy_MiddleBoss::FAttackTask
{
  public:
    FAttackTask_ApproachTarget(AEnemy_MiddleBoss* InSourceBoss, AActor* TargetActor, EAttackType InAttackType, float InPerformRange)
      : FAttackTask(InSourceBoss, InAttackType)
      , m_targetActor{TargetActor}
      , m_performRange{InPerformRange}
    {
      check(TargetActor != nullptr);
    }

    void UpdateTask(float DeltaTime) override final;

  private:
    TWeakObjectPtr<AActor> m_targetActor;
    float m_performRange;
};

class FAttackTask_JumpAttack : public AEnemy_MiddleBoss::FAttackTask
{
  public:
    FAttackTask_JumpAttack(AEnemy_MiddleBoss* InSourceBoss, AActor* TargetActor, float InJumpMoveSpeed)
      : FAttackTask(InSourceBoss, EAttackType::JumpAttack)
      , m_targetActor{TargetActor}
      , m_jumpMoveSpeed{InJumpMoveSpeed}
    {
      check(TargetActor != nullptr);
      m_targetLocation = TargetActor->GetActorLocation();
      SourceBoss->CurrentSpeed = m_jumpMoveSpeed;
    }

    void UpdateTask(float DeltaTime) override final;

  private:
    TWeakObjectPtr<AActor> m_targetActor;
    float m_jumpMoveSpeed;
    FVector m_targetLocation;
};

class FAttackTask_PreSlammed : public AEnemy_MiddleBoss::FAttackTask
{
  public:
    FAttackTask_PreSlammed(AEnemy_MiddleBoss* InSourceBoss)
      : FAttackTask(InSourceBoss, EAttackType::Slammed)
    { }

    void UpdateTask(float DeltaTime) override final;
};  

class FAttackTask_PreRoar : public AEnemy_MiddleBoss::FAttackTask
{
  public:
    FAttackTask_PreRoar(AEnemy_MiddleBoss* InSourceBoss)
      : FAttackTask(InSourceBoss, EAttackType::Roar)
    { }

    void UpdateTask(float DeltaTime) override final;
};  

AEnemy_MiddleBoss::AEnemy_MiddleBoss()
{
}

void AEnemy_MiddleBoss::OnAttackPerformed(EAttackType InAttackType)
{
  K2_OnAttackPerformed(InAttackType);

  Task.Reset();

  switch (InAttackType)
  {
    case EAttackType::Punch:
    {
      Task = ::MakeUnique<FAttackTask_ApproachTarget>(this, TargetActor, EAttackType::Punch, PunchRange);
    }
    break;

    case EAttackType::JumpAttack:
    {
      Task = ::MakeUnique<FAttackTask_ApproachTarget>(this, TargetActor, EAttackType::JumpAttack, JumpAttackRange);
      // TODO Test
      GetCharacterMovement()->SetMovementMode(MOVE_Flying);
    }
    break;
    
    case EAttackType::Slammed:
    {
      Task = ::MakeUnique<FAttackTask_PreSlammed>(this);
    }
    break;
    
    case EAttackType::Roar:
    {
      Task = ::MakeUnique<FAttackTask_PreRoar>(this);
    }
    break;
  }
}

void AEnemy_MiddleBoss::UpdatePreAttack(float DeltaTime)
{
  if (Task.IsValid())
  {
    if (!Task->bIsFinished)
    {
      Task->UpdateTask(DeltaTime);    
    }
    else
    {
      Task.Reset();
    }

  }

  if (ConditionTask.IsValid())
  {
    if (!ConditionTask->bIsFinished)
    {
      ConditionTask->UpdateTask(DeltaTime);
    }
    else
    {
      ConditionTask.Reset();
    }
  }

}

void AEnemy_MiddleBoss::SetTargetActor(AActor* InTargetActor)
{
  if ((InTargetActor == nullptr) || (TargetActor == InTargetActor))
  {
    return;
  }

  TargetActor = InTargetActor;
}

void AEnemy_MiddleBoss::OnPreAttackTaskFinished(EAttackType InAttackType)
{
  K2_OnPreAttackTaskFinished(InAttackType);
  
  if (Task.IsValid())
  {
    Task->bIsFinished = true;
  }

  if (OnPreAttackTaskFinishedEvent.IsBound())
  {
    OnPreAttackTaskFinishedEvent.Broadcast(InAttackType);
  }
 
}

void AEnemy_MiddleBoss::K2_OnAttackFinished()
{
  if (OnAttackFinished.IsBound())
  {
    OnAttackFinished.Broadcast();
  }

  // TODO
  if (GetCharacterMovement()->IsFlying())
  {
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
  }
}

void AEnemy_MiddleBoss::AddAttackTaskCondition(EAttackType InAttackType)
{
  if (ConditionTask.IsValid())
  {
    ConditionTask.Reset();
  }

  switch(InAttackType)
  {
    case EAttackType::JumpAttack:
    {
      ConditionTask = ::MakeUnique<FAttackTask_JumpAttack>(this, TargetActor, JumpMoveSpeed);
    }
    break;
  }

  K2_AddAttackTaskCondition(InAttackType);
}

void AEnemy_MiddleBoss::OnAttackTaskConditionMet(EAttackType InAttackType)
{
  if (ConditionTask.IsValid())
  {
    ConditionTask->bIsFinished = true;
  }

  K2_OnAttackTaskConditionMet(InAttackType);
}

void AEnemy_MiddleBoss::OnDamaged(const ARRanger::Battle::FARDamageResult& InDamageResult)
{
  ReceiveDamage(InDamageResult.Instigator, InDamageResult.FinalDamage);

  if (IsDead())
  {
    ReceiveLaunch(InDamageResult.FinalLaunchDirection);
  }
}

void AEnemy_MiddleBoss::ReceiveLaunch(const FVector& LaunchDirection)
{
  bDeadLaunchHandled = true;

  // 死亡時は大きく吹っ飛ばす
  const float launchPower = 10000.0f;
  FVector DeathImpulse = LaunchDirection;

  TArray<AActor*> actors{};
  UGameplayStatics::GetAllActorsWithTag(this, FName{"Landmark"}, actors);
  if (actors.Num() > 0)
  {
    if (actors[0] != nullptr)
    {
      const FVector relativeOffsetToLogo = FVector{0.0, 80.0, 400.0};
      const FVector pointToHitLandmark = actors[0]->GetActorLocation() + relativeOffsetToLogo * actors[0]->GetActorScale3D();
      DeathImpulse = (pointToHitLandmark - GetActorLocation()).GetSafeNormal();
    }
  }

  DeathImpulse *= launchPower;
  GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
}

void AEnemy_MiddleBoss::PerformDeadStartEffect()
{
  const float hitStopEffectMultiplier = 0.1f;
  UGameplayStatics::SetGlobalTimeDilation(this, hitStopEffectMultiplier);
  FTimerHandle Dummy{};
  GetWorldTimerManager().SetTimer(Dummy, 
  [this]()
  {
    UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
  }, 0.3f, false);
}


void FAttackTask_ApproachTarget::UpdateTask(float DeltaTime)
{
  if (!m_targetActor.IsValid())
  {
    SourceBoss->OnPreAttackTaskFinished(TaskType);
    return;
  }

  FVector Dir = (m_targetActor->GetActorLocation() - SourceBoss->GetActorLocation());
  Dir.Z = 0.f;
  if (!Dir.IsNearlyZero())
  {
    SourceBoss->SetActorRotation(Dir.Rotation());
  }

  if (Dir.Length() <= m_performRange)
  {
    SourceBoss->OnPreAttackTaskFinished(TaskType);
  }
}

void FAttackTask_JumpAttack::UpdateTask(float DeltaTime)
{
  if (!m_targetActor.IsValid())
  {
    SourceBoss->OnAttackTaskConditionMet(TaskType);
    return;
  } 

  FVector Dir = (m_targetLocation - SourceBoss->GetActorLocation());
  Dir.Z = 0.0;
  if (!Dir.IsNearlyZero())
  {
    // We keep its Pitch and Roll
    const FRotator curtRot = SourceBoss->GetActorRotation();
    const float dirYaw = Dir.Rotation().Yaw;
    const FRotator newRot = FRotator{curtRot.Pitch, dirYaw, curtRot.Roll};
    SourceBoss->SetActorRotation(newRot);
  }

  const float Distance = FVector::Dist2D(SourceBoss->GetActorLocation(), m_targetLocation);

  if (Distance > 50.f)
  {
    const FVector destination = SourceBoss->GetActorLocation() + Dir.GetSafeNormal2D() * m_jumpMoveSpeed * DeltaTime;
    SourceBoss->SetActorLocation(destination);
  }
  else
  {
    SourceBoss->CurrentSpeed = 0.f;
    SourceBoss->OnAttackTaskConditionMet(TaskType);
  }
}

void FAttackTask_PreSlammed::UpdateTask(float DeltaTime)
{
  // TODO Do nothing
  SourceBoss->OnPreAttackTaskFinished(TaskType);
  SourceBoss->K2_OnAttackFinished();
}

void FAttackTask_PreRoar::UpdateTask(float DeltaTime)
{
  // TODO Do nothing
  SourceBoss->OnPreAttackTaskFinished(TaskType);
  SourceBoss->K2_OnAttackFinished();
}
