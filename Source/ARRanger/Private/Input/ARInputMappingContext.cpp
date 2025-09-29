/**
 * ARInputMappingContext.cpp
 */

#include "Input/ARInputMappingContext.h"

#include "InputMappingContext.h"

#include "Internal/ARLoggingHeader.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARInputMappingContext)

UInputMappingContext* UARInputMappingContext::FindIMCWithTag(const FGameplayTag& InTag) const
{
  auto searchByTagLambda = [&InTag](const FARInputMappingContextAndTag& Element)
  {
    return Element.InputStateTag.MatchesTagExact(InTag);
  };

  const FARInputMappingContextAndTag* foundElement = InputMappingContexts.FindByPredicate(searchByTagLambda);

  if (foundElement != nullptr)
  {
    return foundElement->InputMapping.LoadSynchronous();
  }

#if WITH_EDITOR

  UE_LOG(LogARInput, Error, TEXT("InputMappingContext with Tag: [%s] is not found."), *InTag.ToString());

#endif 

  return nullptr;
}