// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/ARInputMappingContext.h"

#include "InputMappingContext.h"

UInputMappingContext* UARInputMappingContext::FindIMCWithTag(const FGameplayTag& InTag) const
{
  if (!InTag.IsValid())
  {
    return nullptr;
  }

  auto searchByTagLambda = [&InTag](const FARInputMappingContextAndTag& Element)
  {
    return Element.InputStateTag.MatchesTagExact(InTag);
  };

  const FARInputMappingContextAndTag* foundElement = InputMappingContexts.FindByPredicate(searchByTagLambda);

  return (foundElement != nullptr) ? foundElement->InputMapping.LoadSynchronous() : nullptr;
  
}