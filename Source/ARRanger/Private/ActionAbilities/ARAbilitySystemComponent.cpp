#include "ActionAbilities/ARAbilitySystemComponent.h"

#include "ActionAbilities/ARGameplayAbilityBase.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

namespace ARRanger::Private
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

  // 押されているアビリティ入力を発動バッファに入れる
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

  // 今のフレームに押されたアビリティ入力を発動バッファに入れる
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

  // バッファにあるアビリティの発動を試みる
  for (const FGameplayAbilitySpecHandle& abilitySpecHandleToActivate : s_abilitiesToActivate)
  {
    FGameplayAbilitySpec* abilitySpec = FindAbilitySpecFromHandle(abilitySpecHandleToActivate);
    if (UARGameplayAbilityBase* ARGA = ::Cast<UARGameplayAbilityBase>(abilitySpec->Ability))
    {
      // 発動条件を確認する
      if (ARGA->bNeedActivateCondition)
      {
        bool bCanActivate = false;
        const TArray<FGameplayAbilitySpec>& activatableAbilities = GetActivatableAbilities();

        // TODO ここのループが重いかも
        // TODO 発動したアビリティのTagを保存する方法でも？
        for (const FGameplayAbilitySpec& conditionAbilitySpec : activatableAbilities)
        {
          if ((conditionAbilitySpec.Ability == nullptr) || 
              (conditionAbilitySpec.Ability->GetClass() == ARGA->GetClass()) || 
              !conditionAbilitySpec.IsActive()
             )
          {
            continue;
          }

          // TODO Only use FIRST Tag to check activate condition
          if (ARGA->HasActivateConditionTag(conditionAbilitySpec.Ability->GetAssetTags().First()))
          {            
            // NOTE ここが動かない場合は、GAのInstancingPolicyをInstancedPerActorに設定する
            UGameplayAbility* conditionGAPrimaryInst = conditionAbilitySpec.GetPrimaryInstance();
            if ((conditionGAPrimaryInst != nullptr) && conditionGAPrimaryInst->CanBeCanceled())
            {
              // 発動前提条件のアビリティをキャンセルする
              CancelAbility(conditionAbilitySpec.Ability);
              bCanActivate = true;
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

    const bool bResult = TryActivateAbility(abilitySpecHandleToActivate);

    if (NotifyActivateAbilityResult.IsBound())
    {
      FGameplayTagContainer abilityTags = abilitySpec->Ability->GetAssetTags(); 
      NotifyActivateAbilityResult.Broadcast(this, abilityTags, bResult);
    }
    
    if ((abilitySpec != nullptr) && (abilitySpec->IsActive()) && (!abilitySpec->InputPressed))
    {
      AbilityLocalInputPressed(abilitySpec->InputID);
    }
    
  }

  // アビリティ入力が離れたアビリティに入力が離れたメッセージを送る
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
  using namespace ARRanger::Private;

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

namespace ARRanger::Private
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
