#include "ActionAbilities/Abilities/ARGameplayAbility_Dead.h"

#include "AbilitySystemComponent.h"
#include "Character/ARHealthComponent.h"

UARGameplayAbility_Dead::UARGameplayAbility_Dead()
{
  InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
  bIsCancelable = false;
}

void UARGameplayAbility_Dead::ActivateAbility(
                          const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          const FGameplayEventData* TriggerEventData
                      )
{
  Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

  // 全てのアビリティをキャンセルする
  UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
  ASC->CancelAbilities(nullptr, nullptr, this);

  // 死亡アビリティがキャンセルできないようにする
  SetCanBeCanceled(false);
  
  OnDeadActivated();
}

void UARGameplayAbility_Dead::EndAbility(
                const FGameplayAbilitySpecHandle Handle,
                const FGameplayAbilityActorInfo* ActorInfo,
                const FGameplayAbilityActivationInfo ActivationInfo,
                bool bReplicateEndAbility, bool bWasCancelled
            )
{
  OnDeadEnded();

  Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UARGameplayAbility_Dead::OnDeadActivated()
{
  if (UARHealthComponent* healthComp = UARHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
  {
    healthComp->StartDead();
  }
}

void UARGameplayAbility_Dead::OnDeadEnded()
{
  if (UARHealthComponent* healthComp = UARHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
  {
    healthComp->FinishDead();
  }
}