// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ARHealthComponent.h"

UARHealthComponent::UARHealthComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
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

void UARHealthComponent::HandleHealthChange(AActor* Instigator, float ChangeValue)
{
  const float prevHealth = GetHealth();

  SetHealthInternal(prevHealth + ChangeValue);

  if (OnHealthChanged.IsBound())
  {
    OnHealthChanged.Broadcast(this, Instigator, prevHealth, GetHealth());
  }

  if (IsDead())
  {
    if (OnDead.IsBound())
    {
      OnDead.Broadcast(GetOwner());
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

  if (IsDead())
  {
    if (OnDead.IsBound())
    {
      OnDead.Broadcast(GetOwner());
    }
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

