// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Abilities/ARGameplayAbility_Attack.h"


UARGameplayAbility_Attack::UARGameplayAbility_Attack()
{
  InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UARGameplayAbility_Attack::ActivateAbility(
  const FGameplayAbilitySpecHandle Handle,
  const FGameplayAbilityActorInfo* ActorInfo,
  const FGameplayAbilityActivationInfo ActivationInfo,
  const FGameplayEventData* TriggerEventData
)
{

}

void UARGameplayAbility_Attack::EndAbility(
  const FGameplayAbilitySpecHandle Handle,
  const FGameplayAbilityActorInfo* ActorInfo,
  const FGameplayAbilityActivationInfo ActivationInfo,
  bool bReplicateEndAbility, bool bWasCancelled
)
{

}