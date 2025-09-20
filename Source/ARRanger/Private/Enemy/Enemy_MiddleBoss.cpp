// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy_MiddleBoss.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Enemy/EnemyAttackTypes.h"

class FPreAttackTask_Punch : public AEnemy_MiddleBoss::FPreAttackTask
{
  public:
    FPreAttackTask_Punch(AEnemy_MiddleBoss* InSourceBoss, AActor* TargetActor, float InPerformRange)
      : FPreAttackTask(InSourceBoss, EAttackType::Punch)
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

class FPreAttackTask_JumpAttack : public AEnemy_MiddleBoss::FPreAttackTask
{
  public:
    FPreAttackTask_JumpAttack(AEnemy_MiddleBoss* InSourceBoss, AActor* TargetActor, float InJumpMoveSpeed)
      : FPreAttackTask(InSourceBoss, EAttackType::JumpAttack)
      , m_targetActor{TargetActor}
      , m_jumpMoveSpeed{InJumpMoveSpeed}
    {
      check(TargetActor != nullptr);
      m_targetLocation = TargetActor->GetActorLocation();
    }

    void UpdateTask(float DeltaTime) override final;

  private:
    TWeakObjectPtr<AActor> m_targetActor;
    float m_jumpMoveSpeed;
    FVector m_targetLocation;
};

class FPreAttackTask_Slammed : public AEnemy_MiddleBoss::FPreAttackTask
{
  public:
    FPreAttackTask_Slammed(AEnemy_MiddleBoss* InSourceBoss)
      : FPreAttackTask(InSourceBoss, EAttackType::Slammed)
    { }

    void UpdateTask(float DeltaTime) override final;
};  

class FPreAttackTask_Roar : public AEnemy_MiddleBoss::FPreAttackTask
{
  public:
    FPreAttackTask_Roar(AEnemy_MiddleBoss* InSourceBoss)
      : FPreAttackTask(InSourceBoss, EAttackType::Roar)
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
      Task = ::MakeUnique<FPreAttackTask_Punch>(this, TargetActor, PunchRange);
    }
    break;

    case EAttackType::JumpAttack:
    {
      Task = ::MakeUnique<FPreAttackTask_JumpAttack>(this, TargetActor, JumpMoveSpeed);
    }
    break;
    
    case EAttackType::Slammed:
    {
      Task = ::MakeUnique<FPreAttackTask_Slammed>(this);
    }
    break;
    
    case EAttackType::Roar:
    {
      Task = ::MakeUnique<FPreAttackTask_Roar>(this);
    }
    break;
  }
}

void AEnemy_MiddleBoss::UpdatePreAttack(float DeltaTime)
{
  if (Task.IsValid())
  {
    if (Task->bIsFinished)
    {
      Task.Reset();
      return;
    }

    Task->UpdateTask(DeltaTime);
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

void FPreAttackTask_Punch::UpdateTask(float DeltaTime)
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

void FPreAttackTask_JumpAttack::UpdateTask(float DeltaTime)
{
  if (!m_targetActor.IsValid())
  {
    SourceBoss->OnPreAttackTaskFinished(TaskType);
    return;
  } 

  const FVector Dir = (m_targetLocation - SourceBoss->GetActorLocation()).GetSafeNormal2D();
  float Distance = FVector::Dist2D(SourceBoss->GetActorLocation(), m_targetLocation);

  if (Distance > 50.f)
  {
    SourceBoss->AddMovementInput(Dir, m_jumpMoveSpeed * DeltaTime);
    SourceBoss->CurrentSpeed = m_jumpMoveSpeed;
  }
  else
  {
    SourceBoss->CurrentSpeed = 0.f;
    // TODO Test
    SourceBoss->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
    SourceBoss->OnPreAttackTaskFinished(TaskType);
  }
}

void FPreAttackTask_Slammed::UpdateTask(float DeltaTime)
{
  // TODO Do nothing
  SourceBoss->OnPreAttackTaskFinished(TaskType);
}

void FPreAttackTask_Roar::UpdateTask(float DeltaTime)
{
  // TODO Do nothing
  SourceBoss->OnPreAttackTaskFinished(TaskType);
}