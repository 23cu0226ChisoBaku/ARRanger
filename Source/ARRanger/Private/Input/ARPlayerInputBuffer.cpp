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
  , m_inputTagBuffers{}
  , m_inputStates{}
{ }

void UARPlayerInputBuffer::InitializeInputBuffer(UARInputComponent* InInputComponent)
{ }


void UARPlayerInputBuffer::UninitializeInputBuffer()
{
  for (auto& inputState : m_inputStates)
  {
    inputState.Reset();
  }
}

void UARPlayerInputBuffer::EvaluateBuffer(const AARRangerPlayerController* InPlayerController, const float DeltaTime, const bool bGamePaused)
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
      inputState->Evaluate(InPlayerController, DeltaTime);
    }
  }

  RemoveInputBufferInternal(InPlayerController);

  if (InPlayerController != nullptr)
  {
    UARAbilitySystemComponent* ARASC = InPlayerController->GetARASC();
    if (ARASC != nullptr)
    {
      for (auto& [ inputBufferTag , bufferLeftTime ] : m_inputTagBuffers)
      {
        bufferLeftTime -= DeltaTime;
        
        // Buffer is valid, count it as active input in current frame
        if (bufferLeftTime > 0.0f)
        {
          ARASC->AbilityInputTagPressed(inputBufferTag);
          ConsumeBuffer(inputBufferTag);
        }
        // Remove expired input buffer
        else
        {
          m_removeTags.AddUnique(inputBufferTag);
        }
      }

      RemoveInputBufferInternal(InPlayerController);
    }
  }
}

bool UARPlayerInputBuffer::IsInputBufferValid(const FGameplayTag& InInputTag) const
{
  return m_inputTagBuffers.Contains(InInputTag) ? (m_inputTagBuffers[InInputTag] > 0.0f) : false;
}

void UARPlayerInputBuffer::ClearAllInputs()
{
  for (const auto& [ bufferInputTag, _ ] : m_inputTagBuffers)
  {
    ConsumeBuffer(bufferInputTag);
  }
}

void UARPlayerInputBuffer::HandleInputTagPressed(const FGameplayTag& InInputTag)
{
  // Refresh buffer during input pressed
  if (InputKeepTime > 0.0f)
  {
    // if (m_inputTagBuffers.Contains(InInputTag))
    // {
    //   m_inputTagBuffers[InInputTag] = InputKeepTime;
    // }
    // else
    // {
    //   m_inputTagBuffers.Emplace(InInputTag, InputKeepTime);
    // }

    bool bGenerateNew = true;
    for (const TPimplPtr<ARRanger::Input::FARInputBufferState>& inputState : m_inputStates)
    {
      if (inputState.IsValid())
      {
        if (inputState->m_inputState == ARRanger::Input::FARInputBufferState::InternalState::Pressed && 
            inputState->m_inputTag.MatchesTagExact(InInputTag))
        {
          inputState->SetLifeTime(InputKeepTime);
          bGenerateNew = false;
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

void UARPlayerInputBuffer::HandleInputTagReleased(const FGameplayTag& InInputTag)
{
  if (InputKeepTime > 0.0f)
  {
    bool bGenerateNew = true;
    for (const TPimplPtr<ARRanger::Input::FARInputBufferState>& inputState : m_inputStates)
    {
      if (inputState.IsValid())
      {
        if (inputState->m_inputState == ARRanger::Input::FARInputBufferState::InternalState::Released && 
            inputState->m_inputTag.MatchesTagExact(InInputTag))
        {
          inputState->SetLifeTime(InputKeepTime);
          bGenerateNew = false;
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
  if (m_inputTagBuffers.Contains(InInputTag))
  {
    m_inputTagBuffers[InInputTag] = 0.0f;
  }
}

void UARPlayerInputBuffer::RemoveInputBufferInternal(const AARRangerPlayerController* InPlayerController)
{
  check(InPlayerController != nullptr);
  
  UARAbilitySystemComponent* ARASC = InPlayerController->GetARASC();
  if (ARASC != nullptr)
  {
    // Remove expired buffer
    if (m_removeTags.Num() > 0)
    {
      for (const FGameplayTag& removeTag : m_removeTags)
      {
        if (m_inputTagBuffers.Contains(removeTag))
        {
          ARASC->AbilityInputTagReleased(removeTag);
          m_inputTagBuffers.Remove(removeTag);
        }
      }
  
      m_removeTags.Reset();
    }
  }
}