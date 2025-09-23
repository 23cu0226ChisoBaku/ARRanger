// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"

#include <type_traits>

#include "ARAbilitySystemComponent.generated.h"

#define UE_API ARRANGER_API

struct FARAbilityInputProcessParameter
{
  float DeltaTime = 0.0f;
  bool bGamePaused = false;
};

/**
 * 
 */
UCLASS()
class UARAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

  UE_API UARAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  /**
   * @brief 現在のアビリティがキャンセルできることを通知する
   */
  UE_API void NotifyAbilityCancelable();

  /**
   * @brief 現在のアビリティキャンセルをブロックすることを通知する
   */
  UE_API void NotifyAbilityBlock();

  /**
   * @brief アビリティ入力を処理
   * @param InputProcessParam インプット処理パラメータ
   * @see   FARAbilityInputProcessParameter
   */
  UE_API void ProcessAbilityInputs(const FARAbilityInputProcessParameter& InInputProcessParam);

  UE_API void AbilityInputTagPressed(const FGameplayTag& InTag);
  UE_API void AbilityInputTagReleased(const FGameplayTag& InTag);

  UFUNCTION(BlueprintCallable, Category = "ARRanger|Ability System Component")
  static UE_API UARAbilitySystemComponent* FindARAbilitySystemComponent(AActor* InActor);

  UE_API void ClearAbilityInputStates();

  UFUNCTION(BlueprintCallable, Category = "ARRanger|Ability System Component")
  UE_API void CancleAbilitiesWithCancelableTag(const FGameplayTag& InTag, bool bForceCancel = true);

private:

  static UAbilitySystemComponent* FindAbilitySystemComponentImpl(AActor* InActor);


private:

  TArray<FGameplayAbilitySpecHandle> m_inputPressedSpecHandles;

  TArray<FGameplayAbilitySpecHandle> m_inputReleasedSpecHandles;

  TArray<FGameplayAbilitySpecHandle> m_inputHeldSpecHandles;
};



#undef UE_API