// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Abilities/ARGameplayAbility_Attack.h"

#include "GameFramework/Character.h"


UARGameplayAbility_Attack::UARGameplayAbility_Attack()
{
  InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UARGameplayAbility_Attack::AddAttackRange()
{

}

void UARGameplayAbility_Attack::RemoveAttackRange()
{

}

void UARGameplayAbility_Attack::ActivateAbility(
  const FGameplayAbilitySpecHandle Handle,
  const FGameplayAbilityActorInfo* ActorInfo,
  const FGameplayAbilityActivationInfo ActivationInfo,
  const FGameplayEventData* TriggerEventData
)
{
  Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

  OnAttackAbilityActivated();
}

void UARGameplayAbility_Attack::EndAbility(
  const FGameplayAbilitySpecHandle Handle,
  const FGameplayAbilityActorInfo* ActorInfo,
  const FGameplayAbilityActivationInfo ActivationInfo,
  bool bReplicateEndAbility, bool bWasCancelled
)
{
  Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

  OnAttackAbilityEnded(bWasCancelled);
}

void UARGameplayAbility_Attack::OnAttackAbilityActivated()
{
  UAnimInstance* animInst = FindAnimInstanceOnAvator();
  if (animInst != nullptr)
  {
    animInst->Montage_Play(AttackMontage);
    animInst->OnMontageEnded.AddUniqueDynamic(this, &UARGameplayAbility_Attack::OnAttackMontageEnded);
  }
}

void UARGameplayAbility_Attack::OnAttackAbilityEnded(bool bWasCancelled)
{
  UAnimInstance* animInst = FindAnimInstanceOnAvator();
  if (animInst != nullptr)
  {
    animInst->OnMontageEnded.RemoveDynamic(this, &UARGameplayAbility_Attack::OnAttackMontageEnded);
  }
}

void UARGameplayAbility_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
  // call EndAbility if montage ended
  const bool bWasCancelled = bInterrupted;
  EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

UAnimInstance* UARGameplayAbility_Attack::FindAnimInstanceOnAvator() const
{
  ACharacter* avatarCharacter = ::Cast<ACharacter>(GetAvatarActorFromActorInfo());
  if ((avatarCharacter != nullptr) && (AttackMontage != nullptr))
  {
    if (avatarCharacter->GetMesh() != nullptr)
    {
      return avatarCharacter->GetMesh()->GetAnimInstance();
    }
  }

  return nullptr;
}