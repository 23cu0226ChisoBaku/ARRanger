// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"

#include "ARGameplayAbilityBase.generated.h"

#define UE_API ARRANGER_API

/**
 * 
 */
UCLASS(Abstract)
class UARGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
  UARGameplayAbilityBase();

  UE_API void SetAbilityCancelable();
  
  UE_API void SetAbilityBlock();

  UE_API bool IsAbilityCancelable() const;

  UE_API virtual void ForceCancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility, bool bForceCancel = false);
  /**Start UObject Interface */
  UE_API virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
  /**End UObject Interface */

  /**Start UGameplayAbility Interface */
  UE_API virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
  /**End UGameplayAbility Interface */

public:
  UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Cancelable from beginning"))
  bool bCancelableAfterActivate = true;
	
};

#undef UE_API
