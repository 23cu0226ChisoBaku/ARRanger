#include "ARInputBufferState.h"

#include "Player/ARRangerPlayerController.h"
#include "ActionAbilities/ARAbilitySystemComponent.h"

namespace ARRanger
{

namespace Input
{

TPimplPtr<FARInputBufferState> FARInputBufferState::MakePressedState(const FGameplayTag& InInputTag, float BufferLifeTime)
{ 
  TPimplPtr<FARInputBufferState> newState = MakeInstanceInternal();
  newState->m_bufferLifeTime = BufferLifeTime;
  newState->m_inputTag = InInputTag;
  newState->m_inputState = InternalState::Pressed;

  return newState;
}

TPimplPtr<FARInputBufferState> FARInputBufferState::MakeReleasedState(const FGameplayTag& InInputTag, float BufferLifeTime)
{
  TPimplPtr<FARInputBufferState> newState = MakeInstanceInternal();
  newState->m_inputTag = InInputTag;
  newState->m_bufferLifeTime = BufferLifeTime;
  newState->m_inputState = InternalState::Released;

  return newState;
}

TPimplPtr<FARInputBufferState> FARInputBufferState::MakeInstanceInternal()
{
  return ::MakePimpl<FARInputBufferState>(PrivateToken{});
}


FARInputBufferState::FARInputBufferState(PrivateToken)
  : m_bufferLifeTime{0.0f}
  , m_inputTag{FGameplayTag::EmptyTag}
  , m_inputState{InternalState::Expired}
{ }

FARInputBufferState::~FARInputBufferState() = default;

void FARInputBufferState::Evaluate(const AARRangerPlayerController* InPlayerController, float DeltaTime)
{
  check(InPlayerController != nullptr);
  m_bufferLifeTime -= DeltaTime;

  CheckInputStateValidation();

  UARAbilitySystemComponent* ARASC = InPlayerController->GetARASC();
  if (ARASC != nullptr)
  {
    switch (m_inputState)
    {
      case InternalState::Pressed:
      {
        ARASC->AbilityInputTagPressed(m_inputTag);
      }
      break;

      case InternalState::Released:
      {
        ARASC->AbilityInputTagReleased(m_inputTag);
      }
    }
  }
}

void FARInputBufferState::ExpireInputState()
{
  m_inputState = InternalState::Expired;
}

float FARInputBufferState::GetLifeTime() const
{
  return FMath::Max(m_bufferLifeTime, 0.0f);
}

void FARInputBufferState::SetLifeTime(float NewBufferLifeTime)
{
  m_bufferLifeTime = NewBufferLifeTime;
}

bool FARInputBufferState::IsInputStateExpired() const
{
  return m_inputState == InternalState::Expired;
}

void FARInputBufferState::CheckInputStateValidation()
{
  // Ignore expired input buffer state 
  if (m_inputState == InternalState::Expired)
  {
    return;
  }

  if (m_bufferLifeTime <= 0.0f)
  {
    switch (m_inputState)
    {
      case InternalState::Pressed:
      {
        m_inputState = InternalState::Released;
      }
      break;

      case InternalState::Released:
      {
        m_inputState = InternalState::Expired;
      }
    }
  }
}


} // namespace ARRanger::Input

} // namespace ARRanger

