// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ARHealthComponent.h"

#include "Curves/CurveFloat.h"

#include "ActionAbilities/ARAbilitySystemComponent.h"
#include "ARGameplayTags.h"

void FARHealthRegenerationEntry::ResetRegenerationState()
{
  if (bUseRegenerationSpeedOverrideCurve)
  {
    RegenerationCurveEvaluationTimeValue = 0.0f;
  }

  bEnableRegeneration = false;
  RegenerationDelayTimeCnt = 0.0f;
}

void FARHealthRegenerationEntry::EvaluateRegeneration(float DeltaTime)
{
  if (!bEnableRegeneration)
  {
    // Start regeneration if delay is finished
    RegenerationDelayTimeCnt += DeltaTime;
    if (RegenerationDelayTimeCnt >= RegenerationDelay)
    {
      bEnableRegeneration = true;
    }
  }

  // Evaluate regeneration curve
  if (bEnableRegeneration && bUseRegenerationSpeedOverrideCurve)
  {
    RegenerationCurveEvaluationTimeValue += DeltaTime;
  }
}

float FARHealthRegenerationEntry::GetRegenerationSpeed() const
{
  // Return 0 if it is still in delay state
  if (!bEnableRegeneration)
  {
    return 0.0f;
  }

  if (bUseRegenerationSpeedOverrideCurve)
  {
    UCurveFloat* targetCurve = OverrideCurve.LoadSynchronous();
    if (targetCurve != nullptr)
    {
      return targetCurve->GetFloatValue(RegenerationCurveEvaluationTimeValue);
    }
  }
  // Fallback to use constant value if we dont use curve to evaluate regeneration speed

  return RegenerationSpeed;
  
}

UARHealthComponent::UARHealthComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
}

UARHealthComponent* UARHealthComponent::FindHealthComponent(AActor* OwningActor)
{
  if (OwningActor == nullptr)
  {
    return nullptr;
  }

  return ::Cast<UARHealthComponent>(OwningActor->GetComponentByClass(StaticClass()));
}

void UARHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (bAutoRegenerationEnable)
  {
    if (!IsHealthMax() && bAutoRegeneration)
    {
      RegenerationEntry.EvaluateRegeneration(DeltaTime);
  
      if (RegenerationEntry.bEnableRegeneration)
      {
        const float regenerationHealth = RegenerationEntry.GetRegenerationSpeed() * DeltaTime;
  
        HandleHealthChange(GetOwner(), regenerationHealth);
      }
    }
  }
}

float UARHealthComponent::GetHealth() const
{
  return Health;
}

float UARHealthComponent::GetMaxHealth() const
{
  return MaxHealth;
}

float UARHealthComponent::GetHealthNormalized() const
{
  const bool bIsMaxHealthValid = !FMath::IsNearlyZero(MaxHealth) && !FMath::IsNaN(MaxHealth);
  return bIsMaxHealthValid ? Health / MaxHealth : 0.0f;
}

bool UARHealthComponent::IsDead() const
{
  return Health <= 0.0f;
}

bool UARHealthComponent::IsHealthMax() const
{
  return FMath::IsNearlyEqual(GetHealth(), GetMaxHealth());
}

void UARHealthComponent::HandleHealthChange(AActor* Instigator, float ChangeValue)
{
  const float prevHealth = GetHealth();

  SetHealthInternal(prevHealth + ChangeValue);

  if (OnHealthChanged.IsBound())
  {
    OnHealthChanged.Broadcast(this, Instigator, prevHealth, GetHealth());
  }
}

void UARHealthComponent::HandleOutOfHealth(AActor* OwningActor)
{
  if (OwningActor != GetOwner())
  {
    return;
  }

  if (OwningActor != nullptr)
  {
    UARAbilitySystemComponent* ARASC = UARAbilitySystemComponent::FindARAbilitySystemComponent(OwningActor);
    if (ARASC != nullptr)
    {
      FGameplayTagContainer deadTag;
      deadTag.AddTag(ARRanger::GameplayTags::Ability_Dead);

      ARASC->TryActivateAbilitiesByTag(deadTag);
    }
  }
}

// Called when the game starts
void UARHealthComponent::BeginPlay()
{
  Super::BeginPlay();

  SetHealthInternal(GetMaxHealth());

  if (OnHealthChanged.IsBound())
  {
    OnHealthChanged.Broadcast(this, nullptr, GetHealth(), GetHealth());
  }
}

void UARHealthComponent::SetHealthInternal(float NewHealth)
{
  Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
}

void UARHealthComponent::SetMaxHealthInternal(float NewMaxHealth)
{
  MaxHealth = FMath::Max(NewMaxHealth, 0.0f);
  Health = FMath::Clamp(Health, 0.0f, MaxHealth);
}

void UARHealthComponent::StartDead()
{
  check(GetOwner() != nullptr);

  if (OnDeadEventStarted.IsBound())
  {
    OnDeadEventStarted.Broadcast(GetOwner());
  }
}

void UARHealthComponent::FinishDead()
{
  check(GetOwner() != nullptr);

  if (OnDeadEventFinished.IsBound())
  {
    OnDeadEventFinished.Broadcast(GetOwner());
  }
}

void UARHealthComponent::SetAutoRegenerationEnable(const bool bEnable)
{
  if (bAutoRegenerationEnable != bEnable)
  {
    RegenerationEntry.ResetRegenerationState();
  }
  
  bAutoRegenerationEnable = bEnable;

}