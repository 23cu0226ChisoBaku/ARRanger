// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionAbilities/ARGameplayAbilityBase.h"

#include "ActionAbilities/Abilities/IARGameplayAbilityNotifyInterface.h"

#include "ARGameplayAbility_Attack.generated.h"

/**Forward declaration */
class UAnimInstance;

#define UE_API ARRANGER_API

UCLASS(Abstract)
class UARGameplayAbility_Attack : public UARGameplayAbilityBase,
                                  public IARGameplayAbilityNotifyInterface
{
	GENERATED_BODY()

public:
  UE_API UARGameplayAbility_Attack();

#if WITH_EDITOR

  /**Start UObject Interface */
  UE_API virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override; 
  /**End UObject Interface */

#endif
  
  /**Start IARGameplayAbilityNotifyInterface Interface*/
  UE_API virtual void GANotify_ImpactResult(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const TArray<FGANotify_ImpactResult>& InImpactResults) override;
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

  UFUNCTION()
  void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

  UAnimInstance* FindAnimInstanceOnAvatar() const;

private:

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackMontage;

  // TODO Instead put damage in ability, Maybe it can be put in another structure(GameplayEffect or something else)
  UPROPERTY(EditDefaultsOnly, Category = "Ability|Attack", meta = (AllowPrivateAccess = "true"))
  float AttackDamage;

  UPROPERTY(EditDefaultsOnly, Category = "Ability|Attack")
  bool bClampKnockbackAngle;

  UPROPERTY(EditDefaultsOnly, Category = "Ability|Attack", meta = (ClampMin = 0, ClampMax = 180, EditCondition = "bClampKnockbackAngle == true", EditConditionHides))
  float KnockbackAngleHalfRange;
};

#undef UE_API
