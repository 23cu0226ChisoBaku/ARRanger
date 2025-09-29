/**
 * ARInputConfig.cpp
 */

#include "Input/ARInputConfig.h"

#include "Internal/ARLoggingHeader.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARInputConfig)

UARInputConfig::UARInputConfig(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{ }

const UInputAction* UARInputConfig::FindAbilityInputAction(const FGameplayTag& InInputTag) const
{
  auto searchAbilityInputPred = [&InInputTag](const FARInputAction& Element)
  {
    return Element.InputTag.MatchesTagExact(InInputTag);
  };

  const FARInputAction* foundElement = AbilityInputActions.FindByPredicate(searchAbilityInputPred);
  if (foundElement != nullptr)
  {
    return foundElement->InputAction;
  }
  
#if WITH_EDITOR

  UE_LOG(LogARInput, Error, TEXT("InputAction with Tag: [%s] is not found."), *InInputTag.ToString());

#endif

  return nullptr;
}

const UInputAction* UARInputConfig::FindNativeInputAction(const FGameplayTag& InInputTag) const
{
  auto searchNativeInputPred = [&InInputTag](const FARInputAction& Element)
  {
    return Element.InputTag.MatchesTagExact(InInputTag);
  };

  const FARInputAction* foundElement = NativeInputActions.FindByPredicate(searchNativeInputPred);
  if (foundElement != nullptr)
  {
    return foundElement->InputAction;
  }

#if WITH_EDITOR

  UE_LOG(LogARInput, Error, TEXT("InputAction with Tag: [%s] is not found."), *InInputTag.ToString());

#endif

  return nullptr;
}