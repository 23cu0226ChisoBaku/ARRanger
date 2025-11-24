#include "Input/ARPlayerInputBuffer.h"

#include "Player/ARRangerPlayerController.h"
#include "ActionAbilities/ARAbilitySystemComponent.h"

#include "Internal/Input/ARInputBufferState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARPlayerInputBuffer)

UARPlayerInputBuffer::UARPlayerInputBuffer(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , InputKeepTime{0.0f}
  , bEvaluateIfGamePaused{false}
  , PlayerController{nullptr}
  , m_inputStates{}
{ }

void UARPlayerInputBuffer::Initialize(AARRangerPlayerController* InPlayerController)
{
  if ((PlayerController != nullptr) && (PlayerController != InPlayerController))
  {
    UARAbilitySystemComponent* ARASC = PlayerController->GetARASC();
    if (ARASC != nullptr)
    {
      ARASC->NotifyActivateAbilityResult.RemoveDynamic(this, &ThisClass::OnTryActivateAbilityHandled);
    }
  }
  
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

  PlayerController = nullptr;
}

void UARPlayerInputBuffer::EvaluateBuffer(const float DeltaTime, const bool bGamePaused)
{
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

  // 無効になった入力ステートを削除
  RemoveExpiredInputState();
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
  if (InputKeepTime > 0.0f)
  {
    bool bGenerateNew = true;
 
    for (const TPimplPtr<ARRanger::Input::FARInputBufferState>& inputState : m_inputStates)
    {
      if (inputState.IsValid() && inputState->IsInputTagMatchesExact(InInputTag))
      {
        bGenerateNew = false;

        // 既存の入力ステートを更新
        if (bOverrideInputState || inputState->IsPressedState())
        {
          inputState->MarkAsPressed();
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

        // 既存の入力ステートを更新
        if (bOverrideInputState || inputState->IsReleasedState())
        {
          inputState->MarkAsReleased();
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

    if (inputState->IsInputTagMatchesExact(InInputTag))
    {
      inputState->SetLifeTime(0.0f);
    }
  }
}

void UARPlayerInputBuffer::RemoveExpiredInputState()
{
  int32 idx = 0;
  while (idx < m_inputStates.Num())
  {
    const TPimplPtr<ARRanger::Input::FARInputBufferState>& inputState = m_inputStates[idx];
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
  // 成功したら入力バッファを消耗する
  if (bActivateSuccess)
  {
    // 最初のタグしか使わない
    const FGameplayTag inputTag = InAbilityAssetTags.GetByIndex(0);
    ConsumeBuffer(inputTag);
  }
}