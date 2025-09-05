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
  void BindAbilityActions(const UARInputConfig* InInputConfig, UserClass* UserObject, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);
};

template<typename UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UARInputComponent::BindAbilityActions(const UARInputConfig* InInputConfig, UserClass* UserObject, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
  check(InInputConfig != nullptr);
  for (const FARInputAction& inputAction: InInputConfig->AbilityInputActions)
  {
    if (PressedFunc != nullptr)
    {
      BindAction(inputAction.InputAction, ETriggerEvent::Triggered, UserObject, PressedFunc, inputAction.InputTag);
    }

    if (ReleasedFunc != nullptr)
    {
      BindAction(inputAction.InputAction, ETriggerEvent::Completed, UserObject, ReleasedFunc, inputAction.InputTag);
    }
  }
} 

#undef UE_API