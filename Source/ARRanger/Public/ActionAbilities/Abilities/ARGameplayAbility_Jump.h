/**
 * @file ARGameplayAbility_Jump.h
 * @author MAI ZHICONG
 * @brief ジャンプアビリティ
 */

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
  
  /**
   * @brief アバターActorが着地する時呼ばれるコールバック
   */
  UFUNCTION()
  void OnCharacterLanded(const FHitResult& HitResult);
};

#undef UE_API