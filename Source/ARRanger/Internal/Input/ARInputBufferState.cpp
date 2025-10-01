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

  if (GEngine)
  {
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Pressed state created: InputTag:[%s]."), *InInputTag.ToString()));
  }

  return newState;
}

TPimplPtr<FARInputBufferState> FARInputBufferState::MakeReleasedState(const FGameplayTag& InInputTag, float BufferLifeTime)
{
  TPimplPtr<FARInputBufferState> newState = MakeInstanceInternal();
  newState->m_inputTag = InInputTag;
  newState->m_bufferLifeTime = BufferLifeTime;
  newState->m_inputState = InternalState::Released;

  if (GEngine)
  {
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Released state created: InputTag:[%s]."), *InInputTag.ToString()));
  }

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
  if (InPlayerController == nullptr)
  {
    return;
  }
  
  m_bufferLifeTime -= DeltaTime;

  CheckInputStateValidation();

  UARAbilitySystemComponent* ARASC = InPlayerController->GetARASC();
  if (ARASC != nullptr)
  {
    FString StateStr = TEXT("Expired");
    switch (m_inputState)
    {
      case InternalState::Pressed:
      {
        ARASC->AbilityInputTagPressed(m_inputTag);
        StateStr = TEXT("Pressed");
      }
      break;

      case InternalState::Released:
      {
        ARASC->AbilityInputTagReleased(m_inputTag);
        StateStr = TEXT("Released");
      }
    }

    if (GEngine)
    {
      const FString EvaluateMessage = FString::Printf(TEXT("Input state evaluated. InputTag:[%s]. LifeTime:[%f]. State:[%s]"), *m_inputTag.ToString(), m_bufferLifeTime, *StateStr);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, EvaluateMessage);
    }
  }

}

void FARInputBufferState::ExpireInputState()
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

bool FARInputBufferState::IsInputStateExpired() const
{
  return m_inputState == InternalState::Expired;
}

bool FARInputBufferState::IsInputTagMatchesExact(const FGameplayTag& InInputTag) const
{
  return m_inputTag.MatchesTagExact(InInputTag);
}

void FARInputBufferState::OnPressed()
{
  m_inputState = InternalState::Pressed;
}

void FARInputBufferState::OnReleased()
{
  m_inputState = InternalState::Released;
}


void FARInputBufferState::CheckInputStateValidation()
{
  if (m_bufferLifeTime <= 0.0f)
  {
    ExpireInputState();
  }
}


} // namespace ARRanger::Input

} // namespace ARRanger

