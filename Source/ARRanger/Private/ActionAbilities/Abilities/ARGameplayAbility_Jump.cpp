// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Abilities/ARGameplayAbility_Jump.h"

#include "GameFramework/Character.h"

UARGameplayAbility_Jump::UARGameplayAbility_Jump()
{
  InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UARGameplayAbility_Jump::CanActivateAbility(
                        const FGameplayAbilitySpecHandle Handle, 
                        const FGameplayAbilityActorInfo* ActorInfo, 
                        const FGameplayTagContainer* SourceTags, 
                        const FGameplayTagContainer* TargetTags, 
                        OUT FGameplayTagContainer* OptionalRelevantTags
                    ) const
{
  if ((ActorInfo == nullptr) || !ActorInfo->AvatarActor.IsValid())
  {
    return false;
  }

  const ACharacter* character = ::Cast<ACharacter>(ActorInfo->AvatarActor.Get());
  if ((character == nullptr) || !character->CanJump())
  {
    return false;
  }

  return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UARGameplayAbility_Jump::ActivateAbility(
                        const FGameplayAbilitySpecHandle Handle,
                        const FGameplayAbilityActorInfo* ActorInfo,
                        const FGameplayAbilityActivationInfo ActivationInfo,
                        const FGameplayEventData* TriggerEventData
                    )
{
  CharacterJumpStart();

  Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UARGameplayAbility_Jump::EndAbility(
                        const FGameplayAbilitySpecHandle Handle,
                        const FGameplayAbilityActorInfo* ActorInfo,
                        const FGameplayAbilityActivationInfo ActivationInfo,
                        bool bReplicateEndAbility, bool bWasCancelled
                    )
{
  CharacterJumpEnd();

  Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
} 

void UARGameplayAbility_Jump::CharacterJumpStart()
{
  if (ACharacter* character = ::Cast<ACharacter>(GetAvatarActorFromActorInfo()))
  {
    if (character->IsLocallyControlled() && !character->bPressedJump)
    {
      character->UnCrouch();
      character->Jump();

      character->LandedDelegate.AddUniqueDynamic(this, &ThisClass::OnCharacterLanded);
    }
  }
}

void UARGameplayAbility_Jump::CharacterJumpEnd()
{
  if (ACharacter* character = ::Cast<ACharacter>(GetAvatarActorFromActorInfo()))
  {
    if (character->IsLocallyControlled() && character->bPressedJump)
    {
      character->StopJumping();
      character->LandedDelegate.RemoveDynamic(this, &ThisClass::OnCharacterLanded);
    }
  }
}

// Bind this to OnLanded
void UARGameplayAbility_Jump::OnCharacterLanded(const FHitResult& HitResult)
{
  const bool bReplicateEndAbility = true;
  const bool bWasCanceled = false;
  
  EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCanceled);
}