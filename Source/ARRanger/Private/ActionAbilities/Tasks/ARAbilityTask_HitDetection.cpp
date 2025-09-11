// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Tasks/ARAbilityTask_HitDetection.h"

#include "RangeDetector/Core/PrimitiveDetectorData.h"

struct FARAbilityTask_HitDetection_ActorStrategy
{
  bool operator()(AActor* AvatarActor, const UPrimitiveDetectorData* InDetectorData, UClass* InActorClass, TArray<TObjectPtr<AActor>>& OutResult) const;
};

struct FARAbilityTask_HitDetection_InterfaceStrategy
{
  bool operator()(AActor* AvatarActor, const UPrimitiveDetectorData* InDetectorData, UClass* InInterfaceClass, TArray<TObjectPtr<AActor>>& OutResult) const;
};

UARAbilityTask_HitDetection::UARAbilityTask_HitDetection(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , HitRange{nullptr}
  , m_detectionTargetClass{nullptr}
  , m_duration{0.0f}
  , m_timeDetectionWillEnd{0.0f}
  , m_type{Internal_NotInit}
  , bEndIfHit{false}
{
  bTickingTask = true;
  bSimulatedTask = true;
}

void UARAbilityTask_HitDetection::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent)
{
  Super::InitSimulatedTask(InGameplayTasksComponent);

  const float timeDetectionStarted = GetWorld()->GetTimeSeconds();
  m_timeDetectionWillEnd = timeDetectionStarted + m_duration;
}

UARAbilityTask_HitDetection* UARAbilityTask_HitDetection::HitDetection_Actor(UGameplayAbility* OwningAbility, FName TaskInstanceName, const FARHitDetectionParameter& InHitDetectionParam, TSubclassOf<AActor> TargetActorClass)
{
  UARAbilityTask_HitDetection* myObj = NewAbilityTask<UARAbilityTask_HitDetection>(OwningAbility, TaskInstanceName);

  myObj->HitRange = InHitDetectionParam.HitRange;
  myObj->m_duration = InHitDetectionParam.Duration;
  myObj->bEndIfHit = InHitDetectionParam.bEndIfHit;
  myObj->m_detectionTargetClass = TargetActorClass;
  myObj->m_type = Internal_Actor;

  return myObj;
}

UARAbilityTask_HitDetection* UARAbilityTask_HitDetection::HitDetection_Interface(UGameplayAbility* OwningAbility, FName TaskInstanceName, const FARHitDetectionParameter& InHitDetectionParam, TSubclassOf<UInterface> TargetInterfaceClass)
{
  UARAbilityTask_HitDetection* myObj = NewAbilityTask<UARAbilityTask_HitDetection>(OwningAbility, TaskInstanceName);

  myObj->HitRange = InHitDetectionParam.HitRange;
  myObj->m_duration = InHitDetectionParam.Duration;
  myObj->bEndIfHit = InHitDetectionParam.bEndIfHit;
  myObj->m_detectionTargetClass = TargetInterfaceClass;
  myObj->m_type = Internal_Interface;

  return myObj;
}

void UARAbilityTask_HitDetection::Activate()
{
  if (!CanDetectHit())
  {
    EndHitDetectionTask();
    return;
  }
}

void UARAbilityTask_HitDetection::TickTask(float DeltaTime)
{
  Super::TickTask(DeltaTime);

  float currentTime = GetWorld()->GetTimeSeconds();
  if (currentTime >= m_timeDetectionWillEnd)
  {
    EndHitDetectionTask();
    return;
  }

  bool bIsHit = false;
  static TArray<TObjectPtr<AActor>> hitResult{};
  hitResult.Reset();

  switch (m_type)
  {
    case Internal_Actor:
    {
      bIsHit = FARAbilityTask_HitDetection_ActorStrategy{}(GetAvatarActor(), HitRange, m_detectionTargetClass, hitResult);
    }
    break;

    case Internal_Interface:
    {
      bIsHit = FARAbilityTask_HitDetection_InterfaceStrategy{}(GetAvatarActor(), HitRange, m_detectionTargetClass, hitResult);
    }
    break;

    default:
    {
      EndHitDetectionTask();
      return;
    }
  }

  if (bIsHit)
  {
    OnHit(hitResult);

    if (ShouldBroadcastAbilityTaskDelegates())
    {
      static TArray<AActor*> hitResultPassToBP{};
      hitResultPassToBP.Reset(hitResult.Num());
      for (const TObjectPtr<AActor>& hitActor : hitResult)
      {
        hitResultPassToBP.Add(hitActor);
      }

      OnHitDetected.Broadcast(hitResultPassToBP);
    }

    if (bEndIfHit)
    {
      EndHitDetectionTask();
      return;
    }
  }
}

void UARAbilityTask_HitDetection::OnDestroy(bool AbilityIsEnding)
{
  Super::OnDestroy(AbilityIsEnding);
}

void UARAbilityTask_HitDetection::EndHitDetectionTask()
{
  if (ShouldBroadcastAbilityTaskDelegates())
  {
    OnHitDetectionFinish.Broadcast();
  }
  EndTask();
}

bool UARAbilityTask_HitDetection::CanDetectHit() const
{
  return  (GetAvatarActor() != nullptr) &&
          (HitRange != nullptr) && 
          (m_type != Internal_NotInit) && 
          (m_detectionTargetClass != nullptr);
}

bool FARAbilityTask_HitDetection_ActorStrategy::operator()(AActor* AvatarActor, const UPrimitiveDetectorData* InDetectorData, UClass* InActorClass, TArray<TObjectPtr<AActor>>& OutResult) const
{
  check(AvatarActor != nullptr);
  check(InDetectorData != nullptr);
  check(InActorClass != nullptr);
  OutResult.Reset();

  // (void)InDetectorData->DetectTargets(AvatarActor->GetWorld(), AvatarActor, OutResult);
  if (OutResult.Num() > 0)
  {
    int32 idx = 0;
    while (idx < OutResult.Num())
    {
      const TObjectPtr<AActor> hitActor = OutResult[idx];
      if ((hitActor == nullptr) || !hitActor->IsA(InActorClass))
      {
        OutResult.RemoveAt(idx);
        continue;
      }

      ++idx;
    }
  }

  return OutResult.Num() > 0;
}

bool FARAbilityTask_HitDetection_InterfaceStrategy::operator()(AActor* AvatarActor, const UPrimitiveDetectorData* InDetectorData, UClass* InInterfaceClass, TArray<TObjectPtr<AActor>>& OutResult) const
{
  check(AvatarActor != nullptr);
  check(InDetectorData != nullptr);
  check(InInterfaceClass != nullptr);
  OutResult.Reset();

  // (void)InDetectorData->DetectTargets(AvatarActor->GetWorld(), AvatarActor, OutResult);
  if (OutResult.Num() > 0)
  {
    int32 idx = 0;
    while (idx < OutResult.Num())
    {
      const TObjectPtr<AActor> hitActor = OutResult[idx];
      if ((hitActor == nullptr) || !hitActor->GetClass()->ImplementsInterface(InInterfaceClass))
      {
        OutResult.RemoveAt(idx);
        continue;
      }

      ++idx;
    }
  }

  return OutResult.Num() > 0;
}
