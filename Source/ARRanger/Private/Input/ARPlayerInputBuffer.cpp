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

}

void UARPlayerInputBuffer::UninitializeInputBuffer()
{
  m_inputTagBuffers.Reset();
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
      for (auto& [ inputBufferTag , bufferLeftTime ] : m_inputTagBuffers)
      {
        bufferLeftTime -= DeltaTime;
        
        // Buffer is valid, count it as active input in current frame
        if (bufferLeftTime > 0.0f)
        {
          ARASC->AbilityInputTagPressed(inputBufferTag);
          ConsumeBuffer(inputBufferTag);
        }
        else
        {
          m_removeTags.AddUnique(inputBufferTag);
        }
        
#if WITH_EDITOR
        if (GEngine)
        {
          GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, inputBufferTag.ToString());
        }
#endif
      }


      // Remove expired buffer
      if (m_removeTags.Num() > 0)
      {
        for (const auto& removeTag : m_removeTags)
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

void UARPlayerInputBuffer::HandleInputTagReleased(const FGameplayTag& InInputTag)
{
  // TODO Maybe we should implement something
  ConsumeBuffer(InInputTag);
}

void UARPlayerInputBuffer::ConsumeBuffer(const FGameplayTag& InInputTag)
{
  // TODO Do not remove element here
  if (m_inputTagBuffers.Contains(InInputTag))
  {
    m_inputTagBuffers[InInputTag] = 0.0f;
  }
}