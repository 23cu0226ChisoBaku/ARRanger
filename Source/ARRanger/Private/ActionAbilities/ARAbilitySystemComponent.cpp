// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/ARAbilitySystemComponent.h"

#include "ActionAbilities/ARGameplayAbilityBase.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

namespace
{
  UAbilitySystemComponent* GetASCByInterface_ActorImpl(const AActor* InActor);
  UAbilitySystemComponent* GetASCByInterface_ActorComp(const AActor* InActor);
  UAbilitySystemComponent* GetASCByInterface_PawnController(const APawn* InPawn);
  UAbilitySystemComponent* GetASCByInterface_PlayerState(const APlayerController* InPlayerController);
}

void UARAbilitySystemComponent::NotifyAbilityCancelable()
{
  const TArray<FGameplayAbilitySpec>& allActivatableAbilites = GetActivatableAbilities();
  for (const FGameplayAbilitySpec& activatableAbility : allActivatableAbilites)
  {
    if (activatableAbility.IsActive())
    {
      if (UARGameplayAbilityBase* gameplayAbility = ::Cast<UARGameplayAbilityBase>(activatableAbility.GetPrimaryInstance()))
      {
        gameplayAbility->SetAbilityCancelable();
        break;
      }
    }
  }
}

void UARAbilitySystemComponent::NotifyAbilityBlock()
{
  const TArray<FGameplayAbilitySpec>& allActivatableAbilites = GetActivatableAbilities();
  for (const FGameplayAbilitySpec& activatableAbility : allActivatableAbilites)
  {
    if (activatableAbility.IsActive())
    {
      if (UARGameplayAbilityBase* gameplayAbility = ::Cast<UARGameplayAbilityBase>(activatableAbility.GetPrimaryInstance()))
      {
        gameplayAbility->SetAbilityBlock();
        break;
      }
    }
  }
}

UARAbilitySystemComponent* UARAbilitySystemComponent::FindARAbilitySystemComponent(AActor* InActor)
{
  return ::Cast<UARAbilitySystemComponent>(FindAbilitySystemComponentImpl(InActor));
}

UAbilitySystemComponent* UARAbilitySystemComponent::FindAbilitySystemComponentImpl(AActor* InActor)
{
  if (InActor == nullptr)
  {
    return nullptr;
  }

  // Find ASC on Actor
  UAbilitySystemComponent* result = GetASCByInterface_ActorImpl(InActor);

  // Find ASC on ActorComponent
  if (result == nullptr)
  {
    result = GetASCByInterface_ActorComp(InActor);
  }

  if (result == nullptr)
  {
    // Find ASC on Controller
    if (APawn* pawn = ::Cast<APawn>(InActor))
    {
      result = GetASCByInterface_PawnController(pawn);

      // Find ASC on PlayerState
      if (result == nullptr)
      {
        if (APlayerController* playerController = ::Cast<APlayerController>(pawn->GetController()))
        {
          result = GetASCByInterface_PlayerState(playerController);
        }
      }
    }
  }

  return result;
}

namespace
{
  UAbilitySystemComponent* GetASCByInterface_ActorImpl(const AActor* InActor)
  {
    check(InActor != nullptr);
    if (InActor->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
    {
      return ::Cast<IAbilitySystemInterface>(InActor)->GetAbilitySystemComponent();
    }

    return nullptr;
  }

  UAbilitySystemComponent* GetASCByInterface_ActorComp(const AActor* InActor)
  {
    check(InActor != nullptr);
    TArray<UActorComponent*> components = InActor->GetComponentsByInterface(UAbilitySystemInterface::StaticClass());
    if (components.Num() > 0)
    {
      return ::Cast<IAbilitySystemInterface>(components[0])->GetAbilitySystemComponent();
    }

    return nullptr;
  }

  UAbilitySystemComponent* GetASCByInterface_PawnController(const APawn* InPawn)
  {
    check(InPawn != nullptr);
    AController* pawnController = InPawn->GetController();
    if (pawnController != nullptr && pawnController->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
    {
      return ::Cast<IAbilitySystemInterface>(pawnController)->GetAbilitySystemComponent();
    }

    return nullptr;
  }

  UAbilitySystemComponent* GetASCByInterface_PlayerState(const APlayerController* InPlayerController)
  {
    check(InPlayerController != nullptr);
    APlayerState* playerState = InPlayerController->PlayerState;
    if (playerState != nullptr && playerState->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
    {
      return ::Cast<IAbilitySystemInterface>(playerState)->GetAbilitySystemComponent();
    }

    return nullptr;
  }
}