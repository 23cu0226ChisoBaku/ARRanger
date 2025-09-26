// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Tasks/AT_TickUntilInputRelease.h"

UAT_TickUntilInputRelease::UAT_TickUntilInputRelease(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  bTickingTask = true;
}

void UAT_TickUntilInputRelease::Activate()
{
  Super::Activate();

  UE_LOG(LogTemp, Error, TEXT("TickUntilInputRelease::Activate"));
}

void UAT_TickUntilInputRelease::TickTask(float DeltaTime)
{
  Super::TickTask(DeltaTime);

  if (ShouldBroadcastAbilityTaskDelegates())
  {
    OnTick.Broadcast(DeltaTime);
  }
}

UAT_TickUntilInputRelease* UAT_TickUntilInputRelease::TickUntilInputRelease(UGameplayAbility* OwningAbility)
{
  UAT_TickUntilInputRelease* task = NewAbilityTask<UAT_TickUntilInputRelease>(OwningAbility);
  task->bTickingTask = true;
  return task;
}
