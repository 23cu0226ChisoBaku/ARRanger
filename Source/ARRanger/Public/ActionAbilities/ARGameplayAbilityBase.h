/**
 * @file ARGameplayAbilityBase.h
 * @brief Base class of GameplayAbility for ARRanger project
 */

#pragma once

#include "Abilities/GameplayAbility.h"

#include "ARGameplayAbilityBase.generated.h"

#define UE_API ARRANGER_API

/**Forward declaration */
class AController;

/**
 * @class UARGameplayAbilityBase
 * @brief Base class of GameplayAbility for ARRanger project
 */
UCLASS(Abstract)
class UARGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
  /**
   * @brief Default constructor
   */
  UE_API UARGameplayAbilityBase();

  /**
   * @brief Set the GameplayAbility to cancelable state
   */
  UE_API void SetAbilityCancelable();

  /**
   * @brief Set the GameplayAbility to block cancel state
   */
  UE_API void SetAbilityBlock();

  /**
   * @brief Check whether GameplayAbility is cancelable
   * 
   * @return True if cancelable, otherwise false 
   */
  UE_API bool IsAbilityCancelable() const;

  /**
   * @brief Check whether GameplayAbility has activate condition
   * 
   * @param InTag Tag of GameplayAbility that try to activate
   * @return True if condition tag matches exact. otherwise false
   */
  UE_API bool HasActivateConditionTag(const FGameplayTag& InTag) const;

  /**
   * @brief Check whether GameplayAbility can be canceled by tag
   * 
   * @param InTag Tag to cancel this GameplayAbility
   * @return True if InTag is inside CancleableConditionTags, otherwise false
   */
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

  /**
   * @brief Get asset tags in blueprint
   * @return Result of GetAssetTags()
   */
  UFUNCTION(BlueprintPure, Category = "GameplayAbilityBase", meta = (DisplayName = "Get Default Tags"))
  FGameplayTagContainer K2_GetAssetTags() const;

  /**
   * @brief Get AController of avatar actor
   * @return Valid controller if avatar has one.Maybe nullptr if avatar actor is not pawn or pawn has no valid controller
   */
  UFUNCTION(BlueprintPure, Category = "GameplayAbilityBase", meta = (DisplayName = "Get Controller"))
  AController* GetController() const;

public:
  UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Cancelable from beginning"))
  bool bCancelableAfterActivate = true;

  UPROPERTY(EditDefaultsOnly)
  bool bNeedActivateCondition = false;

  UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bNeedActivateCondition == true", EditConditionHides, DisplayName = "Condition Tag"))
  FGameplayTag ActivateConditionTag;

  UPROPERTY(EditDefaultsOnly, Category = "GameplayAbilityBase", meta = (DisplayName = "Cancle By Tags"))
  FGameplayTagContainer CancleableConditionTags;
	
};

#undef UE_API
