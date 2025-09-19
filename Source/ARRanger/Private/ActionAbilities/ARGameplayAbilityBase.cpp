// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/ARGameplayAbilityBase.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

UARGameplayAbilityBase::UARGameplayAbilityBase()
  : bCancelableAfterActivate{true}
{ 
  bIsCancelable = true;
}

#if WITH_EDITOR

void UARGameplayAbilityBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);

  if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UARGameplayAbilityBase, bCancelableAfterActivate))
  {
    bIsCancelable = bCancelableAfterActivate;
  }
}

#endif

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

bool UARGameplayAbilityBase::IsAssociatedWithTag(const FGameplayTag& InTag) const
{
  return ActivateConditionTag.MatchesTagExact(InTag); 
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
  const bool bCanActivate =  Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

  return bCanActivate;
}

void UARGameplayAbilityBase::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
  Super::InputPressed(Handle, ActorInfo, ActivationInfo);

  K2_InputPressed();
}

void UARGameplayAbilityBase::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
  Super::InputReleased(Handle, ActorInfo, ActivationInfo);
 
  K2_InputReleased();
}

FGameplayTagContainer UARGameplayAbilityBase::K2_GetAssetTags() const
{
  return GetAssetTags();
}

AController* UARGameplayAbilityBase::GetController() const
{
  if (APawn* ownerPawn = ::Cast<APawn>(GetAvatarActorFromActorInfo()))
  {
    return ownerPawn->GetController();
  }
  
  return nullptr;
}

bool UARGameplayAbilityBase::CanCancelByAnyTag(const FGameplayTag& InTag) const
{
  return CancleableConditionTags.HasTagExact(InTag);
}