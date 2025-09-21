// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ARPlayerState.h"
#include "ActionAbilities/ARAbilitySystemComponent.h"
#include "Pawn/ARPawnInitComponent.h"
#include "PlayerComponents/ARChargeAttackComponent.h"
#include "ARRangerPlayerController.h"

AARPlayerState::AARPlayerState(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  AbilitySystemComponent = CreateDefaultSubobject<UARAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
  check(AbilitySystemComponent != nullptr);
  AbilitySystemComponent->SetIsReplicated(true);

  ChargeAttackComponent = CreateDefaultSubobject<UARChargeAttackComponent>(TEXT("ChargeAttackComponent"));
  check(ChargeAttackComponent != nullptr);
}

// TODO Maybe we should not do this in here
void AARPlayerState::OnPlayerStateInitialized(AARRangerPlayerController* InPlayerController)
{
  check(InPlayerController != nullptr);
  if (ChargeAttackComponent != nullptr)
  {
    InPlayerController->OnGameAbilityHeld.BindUObject(ChargeAttackComponent, &UARChargeAttackComponent::EvaluateCharge);
  }
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

void AARPlayerState::BeginPlay()
{
  Super::BeginPlay();
}



