#pragma once

#ifndef _INTERNAL_AR_INPUT_BUFFER_STATE_
#define _INTERNAL_AR_INPUT_BUFFER_STATE_

#define UE_API ARRANGER_API

#include "GameplayTagContainer.h"

class AARRangerPlayerController;

namespace ARRanger
{

namespace Input
{

class FARInputBufferState
{
  friend class UARPlayerInputBuffer;

  enum InternalState
  {
    Pressed,
    Released,
    Expired
  };

  struct PrivateToken
  {

  };

public:

  static UE_API TPimplPtr<FARInputBufferState> MakePressedState(const FGameplayTag& InInputTag, float BufferLifeTime);
  static UE_API TPimplPtr<FARInputBufferState> MakeReleasedState(const FGameplayTag& InInputTag, float BufferLifeTime);

  UE_API ~FARInputBufferState();

  UE_API FARInputBufferState(PrivateToken);

public:
  UE_API void Evaluate(const AARRangerPlayerController* InPlayerController, float DeltaTime);

  UE_API void ExpireInputState();

  UE_API float GetLifeTime() const;

  UE_API void SetLifeTime(float NewBufferLifeTime);

  UE_API bool IsInputStateExpired() const;

private:
  static TPimplPtr<FARInputBufferState> MakeInstanceInternal();

  void CheckInputStateValidation();

private:

  float m_bufferLifeTime;

  FGameplayTag m_inputTag;

  InternalState m_inputState;

};

} // namespace ARRanger::Input

} // namespace ARRanger

#undef UE_API

#endif
