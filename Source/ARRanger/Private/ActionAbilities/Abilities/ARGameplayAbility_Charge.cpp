// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Abilities/ARGameplayAbility_Charge.h"

// TODO Currently in UARGameplayAbility_Attack we play montage too. Maybe we should move this to base class?
#include "GameFramework/Character.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"


UARGameplayAbility_Charge::UARGameplayAbility_Charge()
  : m_startTime{0.0f}
{
  InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UARGameplayAbility_Charge::ActivateAbility(
                          const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          const FGameplayEventData* TriggerEventData
                      )
{
  Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

  // TODO Currently in UARGameplayAbility_Attack we play montage too. Maybe we should move this to base class?
  if (ChargeMontage != nullptr)
  {
    if (ACharacter* character = ::Cast<ACharacter>(GetAvatarActorFromActorInfo()))
    {
      if (USkeletalMeshComponent* meshComp = character->GetMesh())
      {
        if(UAnimInstance* animInst = meshComp->GetAnimInstance())
        {
          animInst->Montage_Play(ChargeMontage);
        }
      }
    }
  }

  m_startTime = GetWorld()->GetTimeSeconds();

  GenerateTasks();

}

void UARGameplayAbility_Charge::EndAbility(
                          const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          bool bReplicateEndAbility, bool bWasCancelled
                      )
{
  Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

  if (bWasCancelled)
  {
    if (OnChargeFinished.IsBound())
    {
      const float elapsedTime = GetHeldTime();
      OnChargeFinished.Broadcast(bWasCancelled, elapsedTime, GetAssetTags());
    }

    if (ChargeMontage != nullptr)
    {
      // TODO Currently in UARGameplayAbility_Attack we stop montage too. Maybe we should move this to base class?
      if (ACharacter* character = ::Cast<ACharacter>(GetAvatarActorFromActorInfo()))
      {
        if (USkeletalMeshComponent* meshComp = character->GetMesh())
        {
          if(UAnimInstance* animInst = meshComp->GetAnimInstance())
          {
            // TODO 意味不明
            animInst->Montage_Stop(0.0f, ChargeMontage);
          }
        }
      }
    }
  }
  
  DiscardTasks();

}

void UARGameplayAbility_Charge::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
  Super::InputReleased(Handle, ActorInfo, ActivationInfo);

  const bool bReplicateEnd = true;
  const bool bWasCanceled = false;
  EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEnd, bWasCanceled);
}

float UARGameplayAbility_Charge::GetHeldTime() const
{
  if (!IsActive())
  {
    return 0.0f;
  }

  const float elapsedTime = GetWorld()->GetTimeSeconds() - m_startTime;
  return elapsedTime;
}

void UARGameplayAbility_Charge::OnCancelableTagAdded()
{
  const bool bReplicateCancel = true;
  CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateCancel);
}

void UARGameplayAbility_Charge::OnEndableTagAdded()
{
  const bool bReplicateEnd = true;
  const bool bWasCanceled = false;
  EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEnd, bWasCanceled);
}

void UARGameplayAbility_Charge::GenerateTasks()
{
  // Generate Cancelable Tasks
  const TArray<FGameplayTag>& cancelableTags = CancelableTags.GetGameplayTagArray();
  for (const FGameplayTag& tag : cancelableTags)
  {
    if (tag.IsValid())
    {
      UAbilityTask_WaitGameplayTagAdded* waitTagAddedTask = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, tag);
      waitTagAddedTask->Added.AddDynamic(this, &UARGameplayAbility_Charge::OnCancelableTagAdded);
      CancelTasks.Emplace(waitTagAddedTask);
    }
  }

  // Generate Endable Tasks
  const TArray<FGameplayTag>& endableTags = EndableTags.GetGameplayTagArray();
  for (const FGameplayTag& tag : endableTags)
  {
    if (tag.IsValid())
    {
      UAbilityTask_WaitGameplayTagAdded* waitTagAddedTask = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, tag);
      waitTagAddedTask->Added.AddDynamic(this, &UARGameplayAbility_Charge::OnEndableTagAdded);
      EndTasks.Emplace(waitTagAddedTask);
    }
  }
}

void UARGameplayAbility_Charge::DiscardTasks()
{
  // Remove all cancel tasks
  for (UAbilityTask_WaitGameplayTagAdded* cancelTask : CancelTasks)
  {
    if (cancelTask != nullptr)
    {
      cancelTask->Added.RemoveDynamic(this, &UARGameplayAbility_Charge::OnCancelableTagAdded);
      cancelTask->EndTask();
    }
  }

  for (UAbilityTask_WaitGameplayTagAdded* endTask : EndTasks)
  {
    if (endTask != nullptr)
    {
      endTask->Added.RemoveDynamic(this, &UARGameplayAbility_Charge::OnEndableTagAdded);
      endTask->EndTask();
    }
  }

  CancelTasks.Reset();
  EndTasks.Reset();
}