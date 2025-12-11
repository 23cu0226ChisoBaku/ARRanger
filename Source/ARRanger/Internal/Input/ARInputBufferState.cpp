#include "ARInputBufferState.h"

#include "Player/ARRangerPlayerController.h"

namespace ARRanger
{

namespace Input
{

TPimplPtr<FARInputBufferState> FARInputBufferState::MakePressedState(const FGameplayTag& InInputTag, float BufferLifeTime)
{ 
  TPimplPtr<FARInputBufferState> newState = MakeInstanceInternal();
  newState->m_bufferLifeTime = BufferLifeTime;
  newState->m_inputTag = InInputTag;
  newState->m_inputState = StateType::Pressed;

  return newState;
}

TPimplPtr<FARInputBufferState> FARInputBufferState::MakeReleasedState(const FGameplayTag& InInputTag, float BufferLifeTime)
{
  TPimplPtr<FARInputBufferState> newState = MakeInstanceInternal();
  newState->m_bufferLifeTime = BufferLifeTime;
  newState->m_inputTag = InInputTag;
  newState->m_inputState = StateType::Released;

  return newState;
}

TPimplPtr<FARInputBufferState> FARInputBufferState::MakeInstanceInternal()
{
  return ::MakePimpl<FARInputBufferState>(PrivateToken{});
}


FARInputBufferState::FARInputBufferState(PrivateToken)
  : m_bufferLifeTime{0.0f}
  , m_inputTag{FGameplayTag::EmptyTag}
  , m_inputState{StateType::Expired}
{ }

FARInputBufferState::~FARInputBufferState() = default;

void FARInputBufferState::Evaluate(AARRangerPlayerController* PlayerController, float DeltaTime)
{
  if (PlayerController == nullptr)
  {
    return;
  }
  
  m_bufferLifeTime -= DeltaTime;

  CheckInputStateValidation();

  switch (m_inputState)
  {
    case Pressed:
    {
      PlayerController->OnAbilityInputTagPressedEvaluated(m_inputTag);
    }
    break;

    case Released:
    {
      PlayerController->OnAbilityInputTagReleasedEvaluated(m_inputTag);
    }
    break;
  }
}

void FARInputBufferState::AdvanceInputState()
{
  switch (m_inputState)
  {
    case Pressed:
    {
      m_inputState = StateType::Released;
    }
    break;

    case Released:
    {
      m_inputState = StateType::Expired;
    }
    break;
  }
}

float FARInputBufferState::GetLifeTime() const
{
  return FMath::Max(m_bufferLifeTime, 0.0f);
}

void FARInputBufferState::SetLifeTime(float NewBufferLifeTime)
{
  m_bufferLifeTime = NewBufferLifeTime;
}

bool FARInputBufferState::IsInputTagMatchesExact(const FGameplayTag& InInputTag) const
{
  return m_inputTag.MatchesTagExact(InInputTag);
}

void FARInputBufferState::MarkAsPressed()
{
  m_inputState = Pressed;
}

void FARInputBufferState::MarkAsReleased()
{
  m_inputState = Released;
}

void FARInputBufferState::CheckInputStateValidation()
{
  if (FMath::IsNearlyZero(GetLifeTime()))
  {
    AdvanceInputState();
  }
}

} // namespace ARRanger::Input

} // namespace ARRanger

