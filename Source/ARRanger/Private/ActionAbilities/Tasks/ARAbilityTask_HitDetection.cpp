// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Tasks/ARAbilityTask_HitDetection.h"

UARAbilityTask_HitDetection::UARAbilityTask_HitDetection(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , HitRange{nullptr}
  , m_duration{0.0f}
  , m_timeDetectionStarted{0.0f}
  , m_timeDetectionWillEnd{0.0f}
{
  bTickingTask = true;
  bSimulatedTask = true;
}

void UARAbilityTask_HitDetection::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent)
{
  Super::InitSimulatedTask(InGameplayTasksComponent);

  m_timeDetectionStarted = GetWorld()->GetTimeSeconds();
  m_timeDetectionWillEnd = m_timeDetectionStarted + m_duration;
}

UARAbilityTask_HitDetection* UARAbilityTask_HitDetection::HitDetection(UGameplayAbility* OwningAbility, FName TaskInstanceName, const FARHitDetectionParameter& InHitDetectionParam)
{
  UARAbilityTask_HitDetection* myObj = NewAbilityTask<UARAbilityTask_HitDetection>(OwningAbility, TaskInstanceName);

  myObj->HitRange = InHitDetectionParam.HitRange;
  myObj->m_duration = InHitDetectionParam.Duration;

  return myObj;
}

void UARAbilityTask_HitDetection::Activate()
{

}

void UARAbilityTask_HitDetection::TickTask(float DeltaTime)
{
  Super::TickTask(DeltaTime);

  if (HitRange == nullptr)
  {
    EndTask(); 
    return;
  }

  float currentTime = GetWorld()->GetTimeSeconds();
  if (currentTime >= m_timeDetectionWillEnd)
  {
    if (ShouldBroadcastAbilityTaskDelegates())
    {
      
    }
  }


}

void UARAbilityTask_HitDetection::OnDestroy(bool AbilityIsEnding)
{
  Super::OnDestroy(AbilityIsEnding);
}

void UARAbilityTask_HitDetection::OnHitDetected(const TArray<TObjectPtr<AActor>>& InHitResult)
{
  if (OnHitDetectedDelegate.IsBound())
  {
    OnHitDetectedDelegate.Broadcast(InHitResult);
  }
}