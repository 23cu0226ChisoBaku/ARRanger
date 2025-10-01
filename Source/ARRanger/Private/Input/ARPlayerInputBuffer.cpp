// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/ARPlayerInputBuffer.h"

#include "Input/ARInputComponent.h"
#include "Player/ARRangerPlayerController.h"
#include "ActionAbilities/ARAbilitySystemComponent.h"

#include "Internal/Input/ARInputBufferState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARPlayerInputBuffer)

UARPlayerInputBuffer::UARPlayerInputBuffer(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , InputComponent{nullptr}
  , PlayerController{nullptr}
  , m_inputStates{}
{ }

void UARPlayerInputBuffer::Initialize(UARInputComponent* InInputComponent, AARRangerPlayerController* InPlayerController)
{
  if ((PlayerController != nullptr) && (PlayerController != InPlayerController))
  {
    UARAbilitySystemComponent* ARASC = PlayerController->GetARASC();
    if (ARASC != nullptr)
    {
      ARASC->NotifyActivateAbilityResult.RemoveDynamic(this, &ThisClass::OnTryActivateAbilityHandled);
    }
  }
  
  InputComponent = InInputComponent;
  PlayerController = InPlayerController;

  if (PlayerController != nullptr)
  {
    UARAbilitySystemComponent* ARASC = PlayerController->GetARASC();
    if (ARASC != nullptr)
    {
      ARASC->NotifyActivateAbilityResult.AddUniqueDynamic(this, &ThisClass::OnTryActivateAbilityHandled);
    }
  }
}


void UARPlayerInputBuffer::Uninitialize()
{
  ClearAllInputs();

  if (PlayerController != nullptr)
  {
    UARAbilitySystemComponent* ARASC = PlayerController->GetARASC();
    if (ARASC != nullptr)
    {
      ARASC->NotifyActivateAbilityResult.RemoveDynamic(this, &ThisClass::OnTryActivateAbilityHandled);
    }
  }
}

void UARPlayerInputBuffer::EvaluateBuffer(const float DeltaTime, const bool bGamePaused)
{
  // Skip evaluation if bEvaluateIfGamePaused is false and bGamePaused is true
  if (!bEvaluateIfGamePaused && bGamePaused)
  {
    return;
  }

  for (const auto& inputState : m_inputStates)
  {
    if (inputState.IsValid())
    {
      inputState->Evaluate(PlayerController, DeltaTime);
    }
  }

  RemoveInputBufferInternal();
}

void UARPlayerInputBuffer::ClearAllInputs()
{
  for (auto& inputState : m_inputStates)
  {
    inputState.Reset();
  }

  m_inputStates.Reset();
}

void UARPlayerInputBuffer::HandleInputTagPressed(const FGameplayTag& InInputTag, bool bOverrideInputState)
{
  // Refresh buffer during input pressed
  if (InputKeepTime > 0.0f)
  {
    bool bGenerateNew = true;
    for (const TPimplPtr<ARRanger::Input::FARInputBufferState>& inputState : m_inputStates)
    {
      if (inputState.IsValid() && inputState->IsInputTagMatchesExact(InInputTag))
      {
        bGenerateNew = false;
        if (bOverrideInputState || inputState->IsPressedState())
        {
          inputState->OnPressed();
          inputState->SetLifeTime(InputKeepTime);
          break;
        }
      }
    }

    if (bGenerateNew)
    {
      m_inputStates.Emplace(ARRanger::Input::FARInputBufferState::MakePressedState(InInputTag, InputKeepTime));
    }
  }
}

void UARPlayerInputBuffer::HandleInputTagReleased(const FGameplayTag& InInputTag, bool bOverrideInputState)
{
  if (InputKeepTime > 0.0f)
  {
    bool bGenerateNew = true;
    for (const TPimplPtr<ARRanger::Input::FARInputBufferState>& inputState : m_inputStates)
    {
      if (inputState.IsValid() && inputState->IsInputTagMatchesExact(InInputTag))
      {
        bGenerateNew = false;
        if (bOverrideInputState || inputState->IsReleasedState())
        {
          inputState->OnReleased();
          inputState->SetLifeTime(InputKeepTime);
          break;
        }
      }
    }
  
    if (bGenerateNew)
    {
      m_inputStates.Emplace(ARRanger::Input::FARInputBufferState::MakeReleasedState(InInputTag, InputKeepTime));
    }
  }
}

void UARPlayerInputBuffer::ConsumeBuffer(const FGameplayTag& InInputTag)
{
  for (const TPimplPtr<ARRanger::Input::FARInputBufferState>& inputState : m_inputStates)
  {
    if (!inputState.IsValid())
    {
      continue;
    }

    if (inputState->m_inputTag.MatchesTagExact(InInputTag))
    {
      inputState->SetLifeTime(0.0f);
    }
  }
}

void UARPlayerInputBuffer::RemoveInputBufferInternal()
{
  int32 idx = 0;
  while (idx < m_inputStates.Num())
  {
    const TPimplPtr<ARRanger::Input::FARInputBufferState>& inputState = m_inputStates[idx];
    // Remove expired buffer
    if (!inputState.IsValid() || inputState->IsInputStateExpired())
    {
      m_inputStates.RemoveAt(idx);
      continue;
    }

    ++idx;
  }
}

void UARPlayerInputBuffer::OnTryActivateAbilityHandled(UARAbilitySystemComponent* InAbilitySystemComponent, FGameplayTagContainer InAbilityAssetTags, bool bActivateSuccess)
{
  if (bActivateSuccess)
  {
    // TODO Currently we only use the first tag of input
    const FGameplayTag inputTag = InAbilityAssetTags.GetByIndex(0);
    ConsumeBuffer(inputTag);

    if (GEngine)
    {
      FString HandledMessage = FString::Printf(TEXT("On Try Activate Ability Handled. InputTag:[%s]"), *inputTag.ToString());
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, HandledMessage);
    }
  }
}