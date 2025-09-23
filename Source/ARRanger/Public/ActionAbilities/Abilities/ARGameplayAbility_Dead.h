// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionAbilities/ARGameplayAbilityBase.h"

#include "ARGameplayAbility_Dead.generated.h"

#define UE_API ARRANGER_API

/**
 * 
 */
UCLASS()
class UARGameplayAbility_Dead : public UARGameplayAbilityBase
{
	GENERATED_BODY()

public:
  UE_API UARGameplayAbility_Dead();

protected:
  
  /**Start UGameplayAbility Interface */
  UE_API virtual void ActivateAbility(
                          const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          const FGameplayEventData* TriggerEventData
                      ) override;

  UE_API virtual void EndAbility(
                          const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          bool bReplicateEndAbility, bool bWasCancelled
                      ) override;
  /**End UGameplayAbility Interface */

private:

  void OnDeadActivated();

  void OnDeadEnded();
	
};

#undef UE_API
