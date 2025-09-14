// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Abilities/ARGameplayAbility_Charge.h"

// TODO Currently in UARGameplayAbility_Attack we play montage too. Maybe we should move this to base class?
#include "GameFramework/Character.h"

#include "Kismet/GameplayStatics.h"

UARGameplayAbility_Charge::UARGameplayAbility_Charge()
  : m_inputPressedTime{0.0f}
  , bInputPressed{false}
{
  InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UARGameplayAbility_Charge::ActivateAbility(
                          const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          const FGameplayEventData* TriggerEventData
                      )
{
  Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

  // TODO Currently in UARGameplayAbility_Attack we play montage too. Maybe we should move this to base class?
  if (ChargeMontage != nullptr)
  {
    if (ACharacter* character = ::Cast<ACharacter>(GetAvatarActorFromActorInfo()))
    {
      if (USkeletalMeshComponent* meshComp = character->GetMesh())
      {
        if(UAnimInstance* animInst = meshComp->GetAnimInstance())
        {
          animInst->Montage_Play(ChargeMontage);
        }
      }
    }
  }
}

void UARGameplayAbility_Charge::EndAbility(
                          const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          bool bReplicateEndAbility, bool bWasCancelled
                      )
{
  Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

  // TODO Do something?
}

float UARGameplayAbility_Charge::GetHeldTime() const
{
  if (!bInputPressed)
  {
    return 0.0f;
  }

  return UGameplayStatics::GetTimeSeconds(GetAvatarActorFromActorInfo()) - m_inputPressedTime;
}

void UARGameplayAbility_Charge::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
  m_inputPressedTime = UGameplayStatics::GetTimeSeconds(GetAvatarActorFromActorInfo());
  bInputPressed = true;

  Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UARGameplayAbility_Charge::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
  Super::InputReleased(Handle, ActorInfo, ActivationInfo);

  m_inputPressedTime = 0.0f;
  bInputPressed = false;
}
