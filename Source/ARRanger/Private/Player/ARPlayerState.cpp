// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ARPlayerState.h"
#include "ActionAbilities/ARAbilitySystemComponent.h"

AARPlayerState::AARPlayerState(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  AbilitySystemComponent = CreateDefaultSubobject<UARAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
  check(AbilitySystemComponent != nullptr);
}

UAbilitySystemComponent* AARPlayerState::GetAbilitySystemComponent() const
{
  return GetARAbilitySystemComponent();
}

void AARPlayerState::PostInitializeComponents()
{
  Super::PostInitializeComponents();

  check(AbilitySystemComponent != nullptr);
  AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

}

