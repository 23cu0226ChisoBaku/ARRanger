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
  UAbilitySystemComponent* GetASCByInterface_PlayerState(const APawn* InPawn);
}

UARAbilitySystemComponent::UARAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , m_inputPressedSpecHandles{}
  , m_inputReleasedSpecHandles{}
  , m_inputHeldSpecHandles{}
{
  ClearAbilityInputStates();
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

void UARAbilitySystemComponent::ProcessAbilityInputs(const FARAbilityInputProcessParameter& InputProcessParam)
{
  static TArray<FGameplayAbilitySpecHandle> s_abilitiesToActivate{};
  s_abilitiesToActivate.Reset();

  // Process held input ability
  for (const FGameplayAbilitySpecHandle& heldSpecHandle : m_inputHeldSpecHandles)
  {
    const FGameplayAbilitySpec* abilitySpec = FindAbilitySpecFromHandle(heldSpecHandle);
    if (abilitySpec != nullptr)
    {
      if ((abilitySpec->Ability != nullptr) && !abilitySpec->IsActive())
      {
        s_abilitiesToActivate.AddUnique(heldSpecHandle);
      }
    }
  }

  // Process abilities that input pressed this frame
  for (const FGameplayAbilitySpecHandle& pressedHandle : m_inputPressedSpecHandles)
  {
    FGameplayAbilitySpec* abilitySpec = FindAbilitySpecFromHandle(pressedHandle);
    if (abilitySpec != nullptr)
    {
      if (abilitySpec->Ability != nullptr)
      {
        if (!abilitySpec->IsActive())
        {
          s_abilitiesToActivate.AddUnique(pressedHandle);
        }
      }
    }
  }

  // Activate abilities from held and pressed
  for (const FGameplayAbilitySpecHandle& abilitySpecHandleToActivate : s_abilitiesToActivate)
  {
    FGameplayAbilitySpec* abilitySpec = FindAbilitySpecFromHandle(abilitySpecHandleToActivate);
    // Check Activation Condition
    if (UARGameplayAbilityBase* ARGA = ::Cast<UARGameplayAbilityBase>(abilitySpec->Ability))
    {
      if (ARGA->bNeedActivateCondition)
      {
        bool bCanActivate = false;
        const TArray<FGameplayAbilitySpec>& activatableAbilities = GetActivatableAbilities();
        for (const FGameplayAbilitySpec& conditionAbilitySpec : activatableAbilities)
        {
          if ((conditionAbilitySpec.Ability == nullptr) || 
              (conditionAbilitySpec.Ability->GetClass() == ARGA->GetClass()) || 
              !conditionAbilitySpec.IsActive()
             )
          {
            continue;
          }

          // FIXME Only use one Tag to check association
          if (ARGA->IsAssociatedWithTag(conditionAbilitySpec.Ability->GetAssetTags().First()))
          {            
            // TODO if this dont work, Use InstancedPerActor
            // FIXME Force to cancel all GA that even not implemented UARGameplayAbilityBase
            UGameplayAbility* conditionGAPrimaryInst = conditionAbilitySpec.GetPrimaryInstance();
            if ((conditionGAPrimaryInst != nullptr) && conditionGAPrimaryInst->CanBeCanceled())
            {
              // TODO Cancel any ability
              CancelAbility(conditionAbilitySpec.Ability);
              bCanActivate = true;
              if (GEngine)
              {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, conditionAbilitySpec.Ability->GetName());
              }
              break;
            }
          }       
        }

        if (!bCanActivate)
        {
          continue;
        }
      }
    }

    // Finally we activate requested abilities
    TryActivateAbility(abilitySpecHandleToActivate);
    
    // Try to trigger InputPressed
    // Only trigger once
    if ((abilitySpec != nullptr) && (abilitySpec->IsActive()) && (!abilitySpec->InputPressed))
    {
      AbilityLocalInputPressed(abilitySpec->InputID);
    }
    
  }

  // Process abilities that input released this frame
  for (const FGameplayAbilitySpecHandle& releasedHandle : m_inputReleasedSpecHandles)
  {
    FGameplayAbilitySpec* abilitySpec = FindAbilitySpecFromHandle(releasedHandle);
    if (abilitySpec != nullptr)
    {
      if (abilitySpec->Ability != nullptr)
      {
        if (abilitySpec->IsActive())
        {
          AbilityLocalInputReleased(abilitySpec->InputID);
        }
      }
    }
  }

  m_inputPressedSpecHandles.Reset();
  m_inputReleasedSpecHandles.Reset();
}

void UARAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InTag)
{
  if (!InTag.IsValid())
  {
    return;
  }

  const TArray<FGameplayAbilitySpec>& abilitySpecs = GetActivatableAbilities();
  for (const FGameplayAbilitySpec& abilitySpec : abilitySpecs)
  {
    if ((abilitySpec.Ability != nullptr))
    {
      if (abilitySpec.Ability->GetAssetTags().HasTagExact(InTag))
      {
        m_inputPressedSpecHandles.AddUnique(abilitySpec.Handle);
        m_inputHeldSpecHandles.AddUnique(abilitySpec.Handle);
      }
    }
  }
}

void UARAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InTag)
{
  if (!InTag.IsValid())
  {
    return;
  }

  const TArray<FGameplayAbilitySpec>& abilitySpecs = GetActivatableAbilities();
  for (const FGameplayAbilitySpec& abilitySpec : abilitySpecs)
  {
    
    if ((abilitySpec.Ability != nullptr))
    {
      if (abilitySpec.Ability->GetAssetTags().HasTagExact(InTag))
      {
        m_inputReleasedSpecHandles.AddUnique(abilitySpec.Handle);
        m_inputHeldSpecHandles.Remove(abilitySpec.Handle);
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

        result = GetASCByInterface_PlayerState(pawn);
      }
    }
  }

  return result;
}

void UARAbilitySystemComponent::ClearAbilityInputStates()
{
  m_inputPressedSpecHandles.Reset();
  m_inputHeldSpecHandles.Reset();
  m_inputReleasedSpecHandles.Reset();
}

void UARAbilitySystemComponent::CancleAbilitiesWithCancelableTag(const FGameplayTag& InTag, bool bForceCancel)
{
  const TArray<FGameplayAbilitySpec>& abilitySpecs = GetActivatableAbilities();
  for (const FGameplayAbilitySpec& abilitySpec : abilitySpecs)
  {
    if ((abilitySpec.Ability == nullptr) || !abilitySpec.IsActive())
    {
      continue;
    }

    if (UARGameplayAbilityBase* abilityBase = ::Cast<UARGameplayAbilityBase>(abilitySpec.Ability))
    {
      if(abilityBase->CanCancelByAnyTag(InTag))
      {
        if (bForceCancel)
        {
          abilityBase->SetAbilityCancelable();
        }
        
        CancelAbility(abilitySpec.Ability);
      }
    }
  }
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

  UAbilitySystemComponent* GetASCByInterface_PlayerState(const APawn* InPawn)
  {
    check(InPawn != nullptr);
    APlayerState* playerState = InPawn->GetPlayerState();
    if (playerState != nullptr && playerState->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
    {
      return ::Cast<IAbilitySystemInterface>(playerState)->GetAbilitySystemComponent();
    }

    return nullptr;
  }
}
