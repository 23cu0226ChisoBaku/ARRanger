// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionAbilities/ARGameplayAbilityBase.h"

#include "ARGameplayAbility_Attack.generated.h"

/**Forward declaration */
class UPrimitiveDetectorData; // 探知範囲データ

/**
 * 
 */
UCLASS(Abstract)
class ARRANGER_API UARGameplayAbility_Attack : public UARGameplayAbilityBase
{
	GENERATED_BODY()

public:
  UARGameplayAbility_Attack();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility, bool bWasCancelled
    ) override;

private:

  UPROPERTY(EditDefaultsOnly, Category = "Ability|Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackMontage;

  // TODO Need Attack Collision
};
