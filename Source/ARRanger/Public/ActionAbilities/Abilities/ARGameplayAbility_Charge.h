// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionAbilities/ARGameplayAbilityBase.h"

#include "ARGameplayAbility_Charge.generated.h"

#define UE_API ARRANGER_API

/**
 * 
 */
UCLASS()
class UARGameplayAbility_Charge : public UARGameplayAbilityBase
{
	GENERATED_BODY()

public:
  UE_API UARGameplayAbility_Charge();

  UFUNCTION(BlueprintPure, Category = "Ability|Charge")
  float GetHeldTime() const;

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

	UE_API virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	UE_API virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;                      
  /**End UGameplayAbility Interface */


private:
  UPROPERTY(EditDefaultsOnly, Category = "Ability|Charge", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ChargeMontage;

  UPROPERTY(EditDefaultsOnly, Category = "Ability|Charge", meta = (AllowPrivateAccess = "true"))
  FGameplayTag NextAbilityTag;

  
  float m_inputPressedTime;
  uint8 bInputPressed : 1;
	
};

#undef UE_API