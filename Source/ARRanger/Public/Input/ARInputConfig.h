/**
 * ARInputConfig.h
 * Input config for ARRanger project use
 */

#pragma once

#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "ARInputConfig.generated.h"

class UInputAction;

/**
 * ARRanger Input action  
 */
USTRUCT(BlueprintType)
struct FARInputAction
{
  GENERATED_BODY()

  /**InputAction */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  TObjectPtr<const UInputAction> InputAction = nullptr;

  /**Signature GameplayTag */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  FGameplayTag InputTag = FGameplayTag::EmptyTag;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  bool bOverrideBufferIfStateChange = true;
};

/**
 * ARRanger input config
 */
UCLASS(BlueprintType, Const)
class ARRANGER_API UARInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

  /**
   * @brief Default constructor
   */
  UARInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  /**
   * @brief Find ability InputAction by signature tag
   * @param InInputTag  Signature tag
   * 
   * @return UInputAction Found UE native InputAction
   */
  UFUNCTION(BlueprintCallable, Category = "InputConfig")
  const UInputAction* FindAbilityInputAction(const FGameplayTag& InInputTag) const;

  /**
   * @brief Find native InputAction by signature tag
   * @param InInputTag  Signature tag
   * 
   * @return UInputAction Found UE native InputAction
   */
  UFUNCTION(BlueprintCallable, CAtegory = "InputConfig")
  const UInputAction* FindNativeInputAction(const FGameplayTag& InInputTag) const;

  /**List of ability input action */
  UPROPERTY(EditDefaultsOnly, Category = "InputConfig", meta = (TitleProperty = "Ability Input: {InputAction} with {InputTag}."))
  TArray<FARInputAction> AbilityInputActions;

  /**List of native input action */
  UPROPERTY(EditDefaultsOnly, Category = "InputConfig", meta = (TitleProperty = "Native Input: {InputAction} with {InputTag}."))
  TArray<FARInputAction> NativeInputActions;
};
