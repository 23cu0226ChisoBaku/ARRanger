// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/ARGameplayAbilityBase.h"

UARGameplayAbilityBase::UARGameplayAbilityBase()
  : bCancelableAfterActivate{true}
{ 
  bIsCancelable = true;
}

void UARGameplayAbilityBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);

  if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UARGameplayAbilityBase, bCancelableAfterActivate))
  {
    bIsCancelable = bCancelableAfterActivate;
  }
}

void UARGameplayAbilityBase::SetAbilityCancelable()
{
  if (CurrentActorInfo != nullptr)
  {
    SetCanBeCanceled(true);
  }
}

void UARGameplayAbilityBase::SetAbilityBlock()
{
  if (CurrentActorInfo != nullptr)
  {
    SetCanBeCanceled(false);
  }
}

bool UARGameplayAbilityBase::IsAbilityCancelable() const
{
  return CanBeCanceled();
}

void UARGameplayAbilityBase::ForceCancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility, bool bForceCancel)
{
  if (bForceCancel)
  {
    SetAbilityCancelable();
  }

  Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

bool UARGameplayAbilityBase::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
  const bool bCanActivate =  Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)
                          && IsAbilityCancelable();

  return bCanActivate;
}