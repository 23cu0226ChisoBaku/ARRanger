// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"

#include "ARGameplayAbilityBase.generated.h"

#define UE_API ARRANGER_API

/**Forward declaration */
class AController;

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

  UE_API bool IsAssociatedWithTag(const FGameplayTag& InTag) const;

  UE_API virtual void ForceCancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility, bool bForceCancel = false);

  UE_API bool CanCancelByAnyTag(const FGameplayTag& InTag) const;

#if WITH_EDITOR

  /**Start UObject Interface */
  UE_API virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
  /**End UObject Interface */

#endif

  /**Start UGameplayAbility Interface */
  UE_API virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	UE_API virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	UE_API virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
  /**End UGameplayAbility Interface */

  UFUNCTION(BlueprintImplementableEvent, Category = "GameplayAbilityBase", meta = (ScriptName = "InputPressed", DisplayName = "Input Pressed"))
  UE_API void K2_InputPressed();

  UFUNCTION(BlueprintImplementableEvent, Category = "GameplayAbilityBase", meta = (ScriptName = "InputReleased", DisplayName = "Input Released"))
  UE_API void K2_InputReleased();

  UFUNCTION(BlueprintPure, Category = "GameplayAbilityBase", meta = (DisplayName = "Get Default Tags"))
  FGameplayTagContainer K2_GetAssetTags() const;

  UFUNCTION(BlueprintPure, Category = "GameplayAbilityBase", meta = (DisplayName = "Get Controller"))
  AController* GetController() const;

public:
  UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Cancelable from beginning"))
  bool bCancelableAfterActivate = true;

  UPROPERTY(EditDefaultsOnly)
  bool bNeedActivateCondition;

  UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bNeedActivateCondition == true", EditConditionHides, DisplayName = "Condition Tag"))
  FGameplayTag ActivateConditionTag;

  UPROPERTY(EditDefaultsOnly, Category = "GameplayAbilityBase", meta = (DisplayName = "Cancle By Tags"))
  FGameplayTagContainer CancleableConditionTags;
	
};

#undef UE_API
