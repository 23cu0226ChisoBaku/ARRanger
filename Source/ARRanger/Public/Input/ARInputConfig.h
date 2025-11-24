/**
 * ARInputConfig.h
 * インプットコンフィグ
 */

#pragma once

#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "ARInputConfig.generated.h"

/**前方宣言 */
class UInputAction;

USTRUCT(BlueprintType)
struct FARInputAction
{
  GENERATED_BODY()

  /**インプットアクションポインター */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  TObjectPtr<const UInputAction> InputAction = nullptr;

  /**インプットTag */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  FGameplayTag InputTag = FGameplayTag::EmptyTag;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  bool bOverrideBufferIfStateChange = true;
};

UCLASS(BlueprintType, Const)
class ARRANGER_API UARInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
  UARInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  /**
   * @brief アビリティInputActionを探す
   * @param InInputTag  アビリティTag
   * @see AbilityInputActions
   */
  UFUNCTION(BlueprintCallable, Category = "InputConfig")
  const UInputAction* FindAbilityInputAction(const FGameplayTag& InInputTag) const;

  /**
   * @brief ネイティブInputActionを探す
   * @param InInputTag  ネイティブアクションTag
   * @see NativeInputActions
   */
  UFUNCTION(BlueprintCallable, CAtegory = "InputConfig")
  const UInputAction* FindNativeInputAction(const FGameplayTag& InInputTag) const;

  UPROPERTY(EditDefaultsOnly, Category = "InputConfig", meta = (TitleProperty = "Ability Input: {InputAction} with {InputTag}."))
  TArray<FARInputAction> AbilityInputActions;

  UPROPERTY(EditDefaultsOnly, Category = "InputConfig", meta = (TitleProperty = "Native Input: {InputAction} with {InputTag}."))
  TArray<FARInputAction> NativeInputActions;
};
