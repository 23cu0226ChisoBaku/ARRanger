// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/ARPlayerInputBuffer.h"
#include "Input/ARInputComponent.h"
#include "Input/ARInputConfig.h"
#include "ARRangerPlayerController.h"
#include "ActionAbilities/ARAbilitySystemComponent.h"

UARPlayerInputBuffer::UARPlayerInputBuffer(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , InputComponent{nullptr}
  , m_inputTagBuffers{}
  , m_bindHandles{}
{ }

void UARPlayerInputBuffer::InitializeInputBuffer(UARInputComponent* InInputComponent, const UARInputConfig& InInputConfig)
{
  check(InInputComponent != nullptr);
  if (InputComponent != InInputComponent)
  {
    // Clear buffers created before
    UninitializeInputBuffer();
  }

  InputComponent = InInputComponent;
  
  for (const FARInputAction& inputAction : InInputConfig.AbilityInputActions)
  {
    if ((inputAction.InputAction != nullptr) && inputAction.InputTag.IsValid())
    {
      // Bind ability actions to buffer
      InputComponent->BindAbilityActions(&InInputConfig, this, &UARPlayerInputBuffer::InputBuffer_AbilityInputTagPressed, &UARPlayerInputBuffer::InputBuffer_AbilityInputTagReleased, m_bindHandles);
    }
  }
}

void UARPlayerInputBuffer::UninitializeInputBuffer()
{
  if ((InputComponent != nullptr) && m_bindHandles.Num() > 0)
  {
    InputComponent->RemoveBindings(m_bindHandles);
  }

  m_inputTagBuffers.Reset();
  InputComponent = nullptr;
}

void UARPlayerInputBuffer::EvaluateBuffer(const AARRangerPlayerController* InPlayerController, const float DeltaTime, const bool bGamePaused)
{
  if (!bEvaluateIfGamePaused && bGamePaused)
  {
    return;
  }

  if (InPlayerController != nullptr)
  {
    UARAbilitySystemComponent* ARASC = InPlayerController->GetARASC();
    if (ARASC != nullptr)
    {
      for (auto& [ inputBufferTag , bufferLeftTime] : m_inputTagBuffers)
      {
        // Ignore invalid buffer
        if (bufferLeftTime <= 0.0f)
        {
          continue;
        }

        bufferLeftTime -= DeltaTime;
        
        // Buffer is valid, count it as active input in current frame
        if (bufferLeftTime > 0.0f)
        {
          ARASC->AbilityInputTagPressed(inputBufferTag);
          bufferLeftTime = 0.0f;
        }
        else
        {
          ARASC->AbilityInputTagReleased(inputBufferTag);
        }
      }
    }
  }
}

bool UARPlayerInputBuffer::IsInputBufferValid(const FGameplayTag& InInputTag) const
{
  return m_inputTagBuffers.Contains(InInputTag) ? (m_inputTagBuffers[InInputTag] > 0.0f) : false;
}

void UARPlayerInputBuffer::InputBuffer_AbilityInputTagPressed(FGameplayTag InInputTag)
{
  // Refresh buffer during input pressed
  if (InputKeepTime > 0.0f)
  {
    if (m_inputTagBuffers.Contains(InInputTag))
    {
      m_inputTagBuffers[InInputTag] = InputKeepTime;
    }
    else
    {
      m_inputTagBuffers.Emplace(InInputTag, InputKeepTime);
    }
  }
}

void UARPlayerInputBuffer::InputBuffer_AbilityInputTagReleased(FGameplayTag InInputTag)
{
  // TODO Maybe we should implement something
}