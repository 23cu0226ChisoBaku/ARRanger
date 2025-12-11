#include "ActionAbilities/Tasks/AT_TickUntilInputRelease.h"

UAT_TickUntilInputRelease::UAT_TickUntilInputRelease(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  bTickingTask = true;
}

void UAT_TickUntilInputRelease::Activate()
{
  Super::Activate();
  TotalTickTime = 0.0f;
}

void UAT_TickUntilInputRelease::TickTask(float DeltaTime)
{
  Super::TickTask(DeltaTime);

  TotalTickTime += DeltaTime;

  if (ShouldBroadcastAbilityTaskDelegates())
  {
    OnTick.Broadcast(DeltaTime, TotalTickTime);
  }
}

UAT_TickUntilInputRelease* UAT_TickUntilInputRelease::TickUntilInputRelease(UGameplayAbility* OwningAbility)
{
  UAT_TickUntilInputRelease* task = NewAbilityTask<UAT_TickUntilInputRelease>(OwningAbility);
  task->bTickingTask = true;
  task->TotalTickTime = 0.0f;
  return task;
}
