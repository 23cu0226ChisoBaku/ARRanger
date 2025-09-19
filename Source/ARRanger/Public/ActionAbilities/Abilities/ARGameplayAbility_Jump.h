// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionAbilities/ARGameplayAbilityBase.h"
#include "ARGameplayAbility_Jump.generated.h"

#define UE_API ARRANGER_API

UCLASS()
class UARGameplayAbility_Jump : public UARGameplayAbilityBase
{
	GENERATED_BODY()
	
public:

  UE_API UARGameplayAbility_Jump();

  /**Start UGameplayAbility Interface */
  UE_API virtual bool CanActivateAbility(
                          const FGameplayAbilitySpecHandle Handle, 
                          const FGameplayAbilityActorInfo* ActorInfo, 
                          const FGameplayTagContainer* SourceTags = nullptr, 
                          const FGameplayTagContainer* TargetTags = nullptr, 
                          OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
                      ) const override;
  
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

  void CharacterJumpStart();

  void CharacterJumpEnd();
  
  // Bind this to LandedDelegate
  UFUNCTION()
  void OnCharacterLanded(const FHitResult& HitResult);
  
};

#undef UE_API