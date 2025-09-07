// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Abilities/ARGameplayAbility_Attack.h"

#include "GameFramework/Character.h"

#include "BattleSystem/IARAttackable.h"
#include "BattleSystem/IARAttackerInterface.h"


UARGameplayAbility_Attack::UARGameplayAbility_Attack()
{
  InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UARGameplayAbility_Attack::AddAttackRange(UPrimitiveDetectorData* RangeData)
{
  // TODO Create Ability Task
}

void UARGameplayAbility_Attack::RemoveAttackRange(UPrimitiveDetectorData* RangeData)
{
  // TODO It is not necessary to remove manually now.So I decided to leave it empty
}

void UARGameplayAbility_Attack::GANotify_ActorArray(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const TArray<TObjectPtr<AActor>>& InActorArray)
{
  AActor* avatarActor = GetAvatarActorFromActorInfo();
  check(avatarActor != nullptr);
  // Make sure Owern of MeshComp is the same Actor as Avatar
  if ((MeshComp == nullptr) || (avatarActor != MeshComp->GetOwner()))
  {
    return;
  }

  IARAttackerInterface* attackerInterface = ::Cast<IARAttackerInterface>(avatarActor);
  for (const TObjectPtr<AActor>& actor : InActorArray)
  {
    IARAttackable* attackable = ::Cast<IARAttackable>(actor);
    if (attackable != nullptr)
    {
      FARAttackParameters attackParam{};
      // TODO Use Avatar location to knockback Target Temporary
      FVector knockbackDir = actor->GetActorLocation() - avatarActor->GetActorLocation();
      // Make it Z to zero so we can only use Direction on XY-Plane to determine knockback Direction
      knockbackDir.Z = 0.0;

      attackParam.Instigator = avatarActor;
      // TODO Stock damage in GA maybe not a great idea
      attackParam.Damage = AttackDamage;
      attackParam.bUseAttackerActor = true;
      attackParam.LaunchDirection = knockbackDir.GetSafeNormal();

      // Apply
      attackable->AttackTarget(attackerInterface, attackParam);
    }
  }
}

void UARGameplayAbility_Attack::ActivateAbility(
  const FGameplayAbilitySpecHandle Handle,
  const FGameplayAbilityActorInfo* ActorInfo,
  const FGameplayAbilityActivationInfo ActivationInfo,
  const FGameplayEventData* TriggerEventData
)
{
  Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

  OnAttackAbilityActivated();
}

void UARGameplayAbility_Attack::EndAbility(
  const FGameplayAbilitySpecHandle Handle,
  const FGameplayAbilityActorInfo* ActorInfo,
  const FGameplayAbilityActivationInfo ActivationInfo,
  bool bReplicateEndAbility, bool bWasCancelled
)
{
  OnAttackAbilityEnded(bWasCancelled);

  Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UARGameplayAbility_Attack::OnAttackAbilityActivated()
{
  UAnimInstance* animInst = FindAnimInstanceOnAvatar();
  if (animInst != nullptr)
  {
    animInst->Montage_Play(AttackMontage);
    animInst->OnMontageEnded.AddUniqueDynamic(this, &UARGameplayAbility_Attack::OnAttackMontageEnded);
  }
}

void UARGameplayAbility_Attack::OnAttackAbilityEnded(bool bWasCancelled)
{
  UAnimInstance* animInst = FindAnimInstanceOnAvatar();
  if (animInst != nullptr)
  {
    animInst->OnMontageEnded.RemoveDynamic(this, &UARGameplayAbility_Attack::OnAttackMontageEnded);
  }
}

void UARGameplayAbility_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
  // Force to cancel ability if montage ended
  const bool bForceCancel = true;
  ForceCancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bForceCancel);
}

UAnimInstance* UARGameplayAbility_Attack::FindAnimInstanceOnAvatar() const
{
  ACharacter* avatarCharacter = ::Cast<ACharacter>(GetAvatarActorFromActorInfo());
  if ((avatarCharacter != nullptr) && (AttackMontage != nullptr))
  {
    if (avatarCharacter->GetMesh() != nullptr)
    {
      return avatarCharacter->GetMesh()->GetAnimInstance();
    }
  }

  return nullptr;
}