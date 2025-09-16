// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/ARInputConfig.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARInputConfig)

UARInputConfig::UARInputConfig(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{ }

const UInputAction* UARInputConfig::FindAbilityInputAction(const FGameplayTag& InInputTag) const
{
  for (const FARInputAction& inputAction : AbilityInputActions)
  {
    if ((inputAction.InputAction != nullptr) && (inputAction.InputTag == InInputTag))
    {
      return inputAction.InputAction;
    }
  }

#if WITH_EDITOR

  UE_LOG(LogTemp, Error, TEXT("InputAction with Tag: [%s] is not found."), *InInputTag.ToString());

#endif

  return nullptr;
}

const UInputAction* UARInputConfig::FindNativeInputAction(const FGameplayTag& InInputTag) const
{
  for (const FARInputAction& inputAction : NativeInputActions)
  {
    if ((inputAction.InputAction != nullptr) && (inputAction.InputTag == InInputTag))
    {
      return inputAction.InputAction;
    }
  }

#if WITH_EDITOR

  UE_LOG(LogTemp, Error, TEXT("InputAction with Tag: [%s] is not found."), *InInputTag.ToString());

#endif

  return nullptr;
}