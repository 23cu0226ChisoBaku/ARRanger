/**
 *  ARInputComponent.h
 *  (ARRanger)インプットコンポーネント
 */

#pragma once

#include "EnhancedInputComponent.h"

#include "Input/ARInputConfig.h"

#include "ARInputComponent.generated.h"

#define UE_API ARRANGER_API

/**
 * @brief ARInputConfigを使ってインプットをバインドする
 */
UCLASS(Config = Input)
class UARInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:

  UE_API UARInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  /**
   * @brief 入力イベントにバインドした関数を全部消す
   * 
   * @param OutBoundHandles BindActionで返されたハンドル
   */
  UE_API void RemoveBindings(TArray<uint32>& OutBoundHandles);

  /**
   * @brief アビリティアクションを入力イベントにバインド
   * 
   * @param InInputConfig インプットコンフィグ
   * @tparam UserObject   PressedFuncとReleasedFuncを呼び出すためのオブジェクトポインター
   * @tparam PressedFunc  ETriggerEvent::Triggeredの時呼び出される関数ポインター
   * @tparam ReleasedFunc ETriggerEvent::Completedの時呼び出される関数ポインター
   * @param OutHandles    入力イベントハンドル
   */
  template<typename UserClass, typename PressedFuncType, typename ReleasedFuncType>
  void BindAbilityActions(const UARInputConfig* InInputConfig, UserClass* UserObject, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& OutHandles);

  /**
   * @brief ネイティブアクションを入力イベントにバインド
   * 
   * @param InInputConfig インプットコンフィグ
   * @param InInputTag      ネイティブ入力Tag
   * @param InTriggerEvent  ネイティブ入力イベントトリガーイベントタイプ
   * @tparam UserObject     TriggeredFuncを呼び出すためのオブジェクトポインター
   * @tparam TriggeredFunc  InTriggerEventの時呼び出される関数ポインタ
   */
  template<typename UserClass, typename TriggeredFuncType>
  void BindNativeAction(const UARInputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent InTriggerEvent, UserClass* UserObject, TriggeredFuncType TriggeredFunc);
};

template<typename UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UARInputComponent::BindAbilityActions(const UARInputConfig* InInputConfig, UserClass* UserObject, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& OutHandles)
{
  check(InInputConfig != nullptr);
  for (const FARInputAction& inputAction: InInputConfig->AbilityInputActions)
  {
    if ((inputAction.InputAction != nullptr) && inputAction.InputTag.IsValid())
    {
      if (PressedFunc != nullptr)
      {
        OutHandles.AddUnique(BindAction(inputAction.InputAction, ETriggerEvent::Triggered, UserObject, PressedFunc, inputAction.InputTag, inputAction.bOverrideBufferIfStateChange).GetHandle());
      }
  
      if (ReleasedFunc != nullptr)
      {
        OutHandles.AddUnique(BindAction(inputAction.InputAction, ETriggerEvent::Completed, UserObject, ReleasedFunc, inputAction.InputTag, inputAction.bOverrideBufferIfStateChange).GetHandle());
      }
    }
  }
} 

template<typename UserClass, typename TriggeredFuncType>
void UARInputComponent::BindNativeAction(const UARInputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent InTriggerEvent, UserClass* UserObject, TriggeredFuncType TriggeredFunc)
{
  check(InInputConfig != nullptr)
  if (const UInputAction* foundIA = InInputConfig->FindNativeInputAction(InInputTag))
  {
    if (TriggeredFunc != nullptr)
    {
      (void)BindAction(foundIA, InTriggerEvent, UserObject, TriggeredFunc, InInputTag);
    }
  }
}

#undef UE_API