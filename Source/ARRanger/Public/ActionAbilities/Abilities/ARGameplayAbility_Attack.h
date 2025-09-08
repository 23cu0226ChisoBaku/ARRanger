// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionAbilities/ARGameplayAbilityBase.h"

#include "ActionAbilities/Abilities/IARGameplayAbilityNotifyInterface.h"

#include "ARGameplayAbility_Attack.generated.h"

/**Forward declaration */
class UPrimitiveDetectorData; // 探知範囲データ
class UAnimInstance;

#define UE_API ARRANGER_API

/**
 * 
 */
UCLASS(Abstract)
class UARGameplayAbility_Attack : public UARGameplayAbilityBase,
                                  public IARGameplayAbilityNotifyInterface
{
	GENERATED_BODY()

public:
  UE_API UARGameplayAbility_Attack();

  UE_API void AddAttackRange(UPrimitiveDetectorData* RangeData);

  UE_API void RemoveAttackRange(UPrimitiveDetectorData* RangeData);

  /**Start IARGameplayAbilityNotifyInterface Interface*/
  UE_API virtual void GANotify_ActorArray(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const TArray<TObjectPtr<AActor>>& InActorArray) override;
  /**End IARGameplayAbilityNotifyInterface Interface */

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

  UAnimInstance* FindAnimInstanceOnAvatar() const;

private:

  UPROPERTY(EditDefaultsOnly, Category = "Ability|Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackMontage;

  // TODO Instead put damage in ability, Maybe it can be put in another structure(GameplayEffect or something else)
  UPROPERTY(EditDefaultsOnly, Category = "Ability|Attack", meta = (AllowPrivateAccess = "true"))
  float AttackDamage;

};

#undef UE_API
