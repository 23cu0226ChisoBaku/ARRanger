/**
 * @file ARGameplayAbilityBase.h
 * @brief ARRanger専用アビリティのベースクラス
 */

#pragma once

#include "Abilities/GameplayAbility.h"

#include "ARGameplayAbilityBase.generated.h"

#define UE_API ARRANGER_API

/**前方宣言 */
class AController;

/**
 * @class UARGameplayAbilityBase
 * @brief ARRanger専用アビリティのベースクラス
 */
UCLASS(Abstract)
class UARGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:

  UE_API UARGameplayAbilityBase();

  /**
   * @brief アビリティがキャンセルできるように設定する
   */
  UE_API void SetAbilityCancelable();

  /**
   * @brief アビリティキャンセル請求をブロックするように設定する
   */
  UE_API void SetAbilityBlock();

  UE_API bool IsAbilityCancelable() const;

  /**
   * @brief アビリティが発動条件Tagを持っているか
   * 
   * @param InTag 発動しようとするアビリティのTag
   * @return 条件Tagと一致すればtrue、それ以外はfalse
   */
  UE_API bool HasActivateConditionTag(const FGameplayTag& InTag) const;

  /**
   * @brief アビリティが特定のTagでキャンセルされることができるか
   * 
   * @param InTag キャンセルTag
   * @return CancleableConditionTagsにあればtrue、それ以外はfalse
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
   * @brief GetAssetTagsのブループリントバージョン
   * @return GetAssetTags()
   */
  UFUNCTION(BlueprintPure, Category = "GameplayAbilityBase", meta = (DisplayName = "Get Default Tags"))
  FGameplayTagContainer K2_GetAssetTags() const;

  /**
   * @brief アバターActorのコントローラーを取得する
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
