/**
 * @file ARAbilitySystemComponent.h
 * @author MAI ZHICONg
 * @brief ARRanger専用アビリティシステムコンポーネント
 */
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

UCLASS()
class UARAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNotifyActivateAbilityResultDelegate, UARAbilitySystemComponent*, InAbilitySystemComponent, FGameplayTagContainer, InAbilityAssetTags, bool, bResult);
  
public:

  UPROPERTY(BlueprintAssignable)
  FNotifyActivateAbilityResultDelegate NotifyActivateAbilityResult;

  UE_API UARAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  /**
   * @brief 現在発動中のアビリティがキャンセルできることを知らせる
   */
  UE_API void NotifyAbilityCancelable();

  /**
   * @brief 現在発動中のアビリティキャンセルをブロックすることを知らせる
   */
  UE_API void NotifyAbilityBlock();

  /**
   * @brief アビリティ入力を処理
   * @param InputProcessParam インプット処理パラメータ
   * @see   FARAbilityInputProcessParameter
   */
  UE_API void ProcessAbilityInputs(const FARAbilityInputProcessParameter& InInputProcessParam);

  /**
   * @brief Ability入力が押された処理
   * @param InTag 
   */
  UE_API void AbilityInputTagPressed(const FGameplayTag& InTag);

  /**
   * @brief Ability入力が離れた処理
   * @param InTag 
   */
  UE_API void AbilityInputTagReleased(const FGameplayTag& InTag);

  UFUNCTION(BlueprintCallable, Category = "ARRanger|Ability System Component")
  static UE_API UARAbilitySystemComponent* FindARAbilitySystemComponent(AActor* InActor);

  /**
   * @brief キャンセルTagを持つ全てのアビリティをキャンセルする
   * 
   * @param InTag
   * @param bForceCancel 強制キャンセルするか
   */
  UFUNCTION(BlueprintCallable, Category = "ARRanger|Ability System Component")
  UE_API void CancleAbilitiesWithCancelableTag(const FGameplayTag& InTag, bool bForceCancel = true);
  
  UE_API void ClearAbilityInputStates();

private:
  static UAbilitySystemComponent* FindAbilitySystemComponentImpl(AActor* InActor);

private:
  TArray<FGameplayAbilitySpecHandle> m_inputPressedSpecHandles;
  TArray<FGameplayAbilitySpecHandle> m_inputReleasedSpecHandles;
  TArray<FGameplayAbilitySpecHandle> m_inputHeldSpecHandles;

};

#undef UE_API