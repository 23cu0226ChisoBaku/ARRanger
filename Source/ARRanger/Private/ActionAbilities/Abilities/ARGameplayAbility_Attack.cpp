// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/Abilities/ARGameplayAbility_Attack.h"

#include "GameFramework/Character.h"

#include "BattleSystem/IARAttackable.h"
#include "BattleSystem/IARAttackerInterface.h"

UARGameplayAbility_Attack::UARGameplayAbility_Attack()
{
  InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

#if WITH_EDITOR

void UARGameplayAbility_Attack::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
  Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif

void UARGameplayAbility_Attack::GANotify_ImpactResult(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const TArray<FGANotify_ImpactResult>& InImpactResults)
{
  AActor* avatarActor = GetAvatarActorFromActorInfo();
  check(avatarActor != nullptr);
  if (avatarActor == nullptr)
  {
    return;
  }

  for (const FGANotify_ImpactResult& result : InImpactResults)
  {

    IARAttackable* attackable = ::Cast<IARAttackable>(result.HitActor);
    if (attackable == nullptr)
    {
      continue;
    }

    FARAttackParameters attackParam{};
    // TODO Use Avatar location to knockback Target Temporary
    FVector knockbackDir = result.HitActor->GetActorLocation() - result.ImpactLocation;
    // Make it Z to zero so we can only use Direction on XY-Plane to determine knockback Direction

    attackParam.Instigator = result.SourceActor;
    // TODO Stock damage in GA maybe not a great idea
    attackParam.Damage = AttackDamage;
    attackParam.bUseAttackerActor = true;
    attackParam.ImpactLocation = result.ImpactLocation;
    // attackParam.LaunchDirection = knockbackDir.GetSafeNormal();

    // Try using SourceActor as attacker 
    IARAttackerInterface* attacker = ::Cast<IARAttackerInterface>(result.SourceActor);

    // If SourceActor is not attacker, use avatar
    if (attacker == nullptr)
    {
      attacker = ::Cast<IARAttackerInterface>(avatarActor);
    }

    // If attacker is still null, try use OccurrenceComp
    if (attacker == nullptr)
    {
      attacker = :: Cast<IARAttackerInterface>(result.OccurrenceComp);
    }

    FVector knockbackDirNorm = knockbackDir.GetSafeNormal();
    // Use KnockbackRange to calculate final launch direction
    if (bClampKnockbackAngle)
    {
      // Calculate LaunchDir's projection vector on Plane that attacker's up vector is normal vector
      FVector norm{ForceInitToZero};
      FVector attackerFwdDir{ForceInitToZero};
      if ((attacker != nullptr) && (attacker->GetActor() != nullptr))
      {
        norm = attacker->GetActor()->GetActorUpVector();
        attackerFwdDir = attacker->GetActor()->GetActorForwardVector();
      }
      // Fallback to SourceActor
      else if (result.SourceActor != nullptr)
      {
        norm = result.SourceActor->GetActorUpVector();
        attackerFwdDir = result.SourceActor->GetActorForwardVector();
      }
      // Fallback to OccurrenceComp
      else if (result.OccurrenceComp != nullptr)
      {
        norm = result.OccurrenceComp->GetUpVector();
        attackerFwdDir = result.OccurrenceComp->GetForwardVector();
      }
      // Finally, Fallback to avatar
      else
      {
        norm = avatarActor->GetActorUpVector();
        attackerFwdDir = avatarActor->GetActorForwardVector();
      }
  
      norm.Normalize();
      attackerFwdDir.Normalize();
  
      const float projectionAngleCos = FVector::DotProduct(norm, knockbackDirNorm); 
      const FVector projectionVec = knockbackDir - (projectionAngleCos * knockbackDir.Length()) * norm;
      const float curtKnockbackRangeCos = FVector::DotProduct(attackerFwdDir, projectionVec.GetSafeNormal());
      float curtKnockbackRangeAngleDeg = FMath::RadiansToDegrees(FMath::Acos(curtKnockbackRangeCos));
      // Clamp current radian
      curtKnockbackRangeAngleDeg = FMath::Clamp(curtKnockbackRangeAngleDeg, 0.0f, KnockbackAngleHalfRange);
      // Change knockbackDirNorm
      if (!FMath::IsNearlyEqual(FMath::Cos(curtKnockbackRangeAngleDeg), curtKnockbackRangeCos))
      {
        const float rotateAngleSign = FVector::DotProduct(FVector::CrossProduct(attackerFwdDir, projectionVec).GetSafeNormal(), norm);
        knockbackDirNorm = attackerFwdDir.RotateAngleAxis(FMath::Sign(rotateAngleSign) * curtKnockbackRangeAngleDeg, norm).GetSafeNormal();
      }
    }

    // Finally we put knockbackDirNorm to attackParam
    attackParam.LaunchDirection = knockbackDirNorm;
    
    // Apply Attack
    attackable->AttackTarget(attacker, attackParam);
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

  UE_LOG(LogTemp, Error, TEXT("Activate [%s]."), *GetAssetTags().ToString());

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
    if (!bWasCancelled)
    {
      // TODO 
      animInst->Montage_Stop(0.0f, AttackMontage);
    }
    animInst->OnMontageEnded.RemoveDynamic(this, &UARGameplayAbility_Attack::OnAttackMontageEnded);
  }
}

void UARGameplayAbility_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
  if (AttackMontage == Montage)
  {
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
  }
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