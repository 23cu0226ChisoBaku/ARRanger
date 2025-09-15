// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionAbilities/ARGameplayAbilityBase.h"

#include "ARGameplayAbility_Charge.generated.h"

#define UE_API ARRANGER_API


class UAbilityTask_WaitGameplayTagAdded;

/**
 * 
 */
UCLASS()
class UARGameplayAbility_Charge : public UARGameplayAbilityBase
{
  GENERATED_BODY()
  
public:
  UE_API UARGameplayAbility_Charge();
  
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnChargeFinishedDelegate, bool, bWasCanceled, float, TimeHeld, FGameplayTagContainer, ChargeAbilityTags);
  UPROPERTY(BlueprintAssignable)
  FOnChargeFinishedDelegate OnChargeFinished;

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

  // Only for players
  UE_API virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
  /**End UGameplayAbility Interface */

  UFUNCTION(BlueprintPure, Category = "Ability|ChargeInternal")
  float GetHeldTime() const;

private:

  UFUNCTION()
  void OnCancelableTagAdded();

  UFUNCTION()
  void OnEndableTagAdded();

  void GenerateTasks();

  void DiscardTasks();

private:
  UPROPERTY(EditDefaultsOnly, Category = "Ability|Charge", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ChargeMontage;

  UPROPERTY(EditDefaultsOnly, Category = "Ability|Charge", meta = (AllowPrivateAccess = "true", DisplayName = "Cancel Tags When Added"))
  FGameplayTagContainer CancelableTags;

  UPROPERTY(EditDefaultsOnly, Category = "Ability|Charge", meta = (AllowPrivateAccess = "true", DisplayName = "End Tags When Added"))
  FGameplayTagContainer EndableTags;
  
  UPROPERTY()
  TArray< TObjectPtr< UAbilityTask_WaitGameplayTagAdded > > CancelTasks;

  UPROPERTY()
  TArray< TObjectPtr< UAbilityTask_WaitGameplayTagAdded > > EndTasks;

  float m_startTime;

};

#undef UE_API