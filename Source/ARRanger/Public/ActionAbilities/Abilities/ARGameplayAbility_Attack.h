// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionAbilities/ARGameplayAbilityBase.h"

#include "ARGameplayAbility_Attack.generated.h"

/**Forward declaration */
class UPrimitiveDetectorData; // 探知範囲データ
class UAnimInstance;

#define UE_API ARRANGER_API

/**
 * 
 */
UCLASS(Abstract)
class UARGameplayAbility_Attack : public UARGameplayAbilityBase
{
	GENERATED_BODY()

public:
  UE_API UARGameplayAbility_Attack();

  UE_API void AddAttackRange();

  UE_API void RemoveAttackRange();

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
  void OnAttackAbilityActivated();
  void OnAttackAbilityEnded(bool bWasCancelled);

  void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

  UAnimInstance* FindAnimInstanceOnAvator() const;

private:

  UPROPERTY(EditDefaultsOnly, Category = "Ability|Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackMontage;

  // TODO Need Attack Collision
};
