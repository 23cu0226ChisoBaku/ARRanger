// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/ARGameplayAbilityBase.h"

UARGameplayAbilityBase::UARGameplayAbilityBase()
{ 
  SetCanBeCanceled(bCancelableAfterActivate);
}

void UARGameplayAbilityBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UARGameplayAbilityBase, bCancelableAfterActivate))
  {
    SetCanBeCanceled(bCancelableAfterActivate);
  }
}

void UARGameplayAbilityBase::SetAbilityCancelable()
{
  SetCanBeCanceled(true);
}

void UARGameplayAbilityBase::SetAbilityBlock()
{
  SetCanBeCanceled(false);
}

bool UARGameplayAbilityBase::IsAbilityCancelable() const
{
  return CanBeCanceled();
}

bool UARGameplayAbilityBase::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
  const bool bCanActivate =  Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)
                          && IsAbilityCancelable();

  return bCanActivate;
}