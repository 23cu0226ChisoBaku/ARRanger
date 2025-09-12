// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Attributes/ARAttributeSet.h"

#include "ActionAbilities/ARAbilitySystemComponent.h"

UARAttributeSet::UARAttributeSet(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{

}

UWorld* UARAttributeSet::GetWorld() const
{
  const UObject* outer = GetOuter();
  check(outer != nullptr);

  return outer->GetWorld();
}

UARAbilitySystemComponent* UARAttributeSet::GetARAbilitySystemComponent() const
{
  return ::Cast<UARAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}