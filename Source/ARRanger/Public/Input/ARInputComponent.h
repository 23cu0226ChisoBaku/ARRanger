// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnhancedInputComponent.h"
#include "Input/ARInputConfig.h"

#include "ARInputComponent.generated.h"

#define UE_API ARRANGER_API

UCLASS(Config = Input)
class UARInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:

  UE_API UARInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  UE_API void RemoveBindings(TArray<uint32>& OutBoundHandles);

  template<typename UserClass, typename PressedFuncType, typename ReleasedFuncType>
  void BindAbilityActions(const UARInputConfig* InInputConfig, UserClass* UserObject, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& OutHandles);

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
      BindAction(foundIA, InTriggerEvent, UserObject, TriggeredFunc, InInputTag);
    }
  }
}

#undef UE_API