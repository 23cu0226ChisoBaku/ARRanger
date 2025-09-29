/**
 *  ARInputComponent.h
 *  Input component for ARRanger project use
 */

#pragma once

#include "EnhancedInputComponent.h"

#include "Input/ARInputConfig.h"

#include "ARInputComponent.generated.h"

#define UE_API ARRANGER_API

/**
 * @brief Input component for binding specific input event with ARInputConfig
 */
UCLASS(Config = Input)
class UARInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:

  /**
   * @brief Default constructor of UE
   */
  UE_API UARInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  /**
   * @brief Remove input bindings with handles
   * 
   * @param OutBoundHandles Handles from BindAction
   */
  UE_API void RemoveBindings(TArray<uint32>& OutBoundHandles);

  /**
   * @brief Bind ability actions to InputAction in UARInputConfig
   * 
   * @param InInputConfig Input config used by ARRanger project
   * @tparam UserObject   User(class instance) pointer for PressedFuncType and ReleasedFuncType call
   * @tparam PressedFunc  Function to call with input state ETriggerEvent::Triggered
   * @tparam ReleasedFunc Function to call with input state ETriggerEvent::Completed
   * @param OutHandles    Handles of bound event
   */
  template<typename UserClass, typename PressedFuncType, typename ReleasedFuncType>
  void BindAbilityActions(const UARInputConfig* InInputConfig, UserClass* UserObject, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& OutHandles);

  /**
   * @brief Bind native actions to InputAction
   * 
   * @param InInputConfig   Input config used by ARRanger project
   * @param InInputTag      Signature tag of native input
   * @param InTriggerEvent  Native input trigger state
   * @tparam UserObject     User(class instance) pointer for TriggeredFunc call
   * @tparam TriggeredFunc  Function to call with input state InTriggerEvent
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
        OutHandles.AddUnique(BindAction(inputAction.InputAction, ETriggerEvent::Triggered, UserObject, PressedFunc, inputAction.InputTag).GetHandle());
      }
  
      if (ReleasedFunc != nullptr)
      {
        OutHandles.AddUnique(BindAction(inputAction.InputAction, ETriggerEvent::Completed, UserObject, ReleasedFunc, inputAction.InputTag).GetHandle());
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