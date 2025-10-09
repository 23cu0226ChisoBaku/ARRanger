/**
 * @file ARGameplayAbility_Attack.h
 * @brief Attack base class of ARRanger project
 */

#pragma once

#include "ActionAbilities/ARGameplayAbilityBase.h"

#include "ActionAbilities/Abilities/IARGameplayAbilityNotifyInterface.h"

#include "ARGameplayAbility_Attack.generated.h"

/**Forward declaration */
class UAnimInstance;

#define UE_API ARRANGER_API

/**
 * @class UARGameplayAbility_Attack
 * @brief Attack base class of ARRanger project
 */
UCLASS(Abstract)
class UARGameplayAbility_Attack : public UARGameplayAbilityBase,
                                  public IARGameplayAbilityNotifyInterface
{
	GENERATED_BODY()

public:
  /**
   * @brief Default constructor
   */
  UE_API UARGameplayAbility_Attack();
  
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
  /**
   * @brief Process after ActivateAbility called
   */
  void OnAttackAbilityActivated();

  /**
   * @brief Process after EndAbility called
   * @param bWasCancelled Is ability canceled
   */
  void OnAttackAbilityEnded(bool bWasCancelled);

  /**
   * @brief Function to bind to delegate OnMontageEnded in AnimInstance
   */
  UFUNCTION()
  void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

  /**
   * @brief Find AnimInstance in avatar actor
   * @return Valid AnimInstance if avatar actor has one, otherwise nullptr
   */
  UAnimInstance* FindAnimInstanceOnAvatar() const;

private:

  /**Montage to play during attack ability */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackMontage;

  // TODO Instead put damage in ability, Maybe it can be put in another structure(GameplayEffect or something else)
  /**Attack damage */
  UPROPERTY(EditDefaultsOnly, Category = "Ability|Attack", meta = (AllowPrivateAccess = "true"))
  float AttackDamage;

  /**Flag if attack knock back has range */
  UPROPERTY(EditDefaultsOnly, Category = "Ability|Attack")
  bool bClampKnockbackAngle;

  /**Knock back range half angle(Degrees) */
  UPROPERTY(EditDefaultsOnly, Category = "Ability|Attack", meta = (ClampMin = 0, ClampMax = 180, EditCondition = "bClampKnockbackAngle == true", EditConditionHides))
  float KnockbackAngleHalfRange;
};

#undef UE_API
