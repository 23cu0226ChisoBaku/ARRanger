#pragma once

#ifndef _INTERNAL_AR_INPUT_BUFFER_STATE_
#define _INTERNAL_AR_INPUT_BUFFER_STATE_

#define UE_API ARRANGER_API

#include "GameplayTagContainer.h"

/**Forward declaration */
class AARRangerPlayerController;

namespace ARRanger
{

namespace Input
{

/**
 * @brief Input buffer state class for ARRanger project
 */
class FARInputBufferState final
{
  enum InternalState
  {
    Pressed,  // 押された
    Released, // 離された
    Expired   // 有効切れ
  };

  // Private token to avoid access from outside.
  struct PrivateToken{ };

public:

  /**
   * @brief Factory method to create a pressed input state
   * @param InInputTag Input signature tag
   * @param BufferLifeTime Input state life time
   * 
   * @return Created pressed input state 
   */
  static UE_API TPimplPtr<FARInputBufferState> MakePressedState(const FGameplayTag& InInputTag, float BufferLifeTime);
  
  /**
   * @brief Factory method to create a released input state
   * @param InInputTag Input signature tag
   * @param BufferLifeTime Input state life time
   * 
   * @return Created released input state 
   */ 
  static UE_API TPimplPtr<FARInputBufferState> MakeReleasedState(const FGameplayTag& InInputTag, float BufferLifeTime);

  /**
   * @brief Default constructor
   * @param PrivateToken Token to avoid access from outside
   */
  UE_API FARInputBufferState(PrivateToken);
  
  /**
   * @brief Destructor
   */
  UE_API ~FARInputBufferState();

  /**
   * @brief Evaluate input state
   * @param InPlayerController
   * @param DeltaTime
   */
  UE_API void Evaluate(AARRangerPlayerController* InPlayerController, float DeltaTime);

  /**
   * @brief Expire this input state.
   */
  UE_API void ExpireInputState();

  /**
   * @brief Get input state life time
   * @return Life time(Always greater equals than 0.0f)
   */
  UE_API float GetLifeTime() const;

  /**
   * @brief Set input state life time
   * @param NewBufferLifeTime
   */
  UE_API void SetLifeTime(float NewBufferLifeTime);

  /**
   * @brief Check this input state is expired
   * 
   * @return True if expired, otherwise false
   */
  UE_API bool IsInputStateExpired() const;

  /**
   * @brief Check input tag of this input state matches
   * @param InInputTag
   * @return True if input tag of this input state matches exactly to InInputTag,otherwise false.
   */
  UE_API bool IsInputTagMatchesExact(const FGameplayTag& InInputTag) const;

  /**
   * @brief Check if this is pressed input state
   * @return True if input state is Pressed, otherwise false.
   */
  bool IsPressedState() const { return m_inputState == Pressed; } 

  /**
   * @brief Check if this is released input state
   * @return True if input state is Released, otherwise false.
   */
  bool IsReleasedState() const { return m_inputState == Released; } 

  /**
   * @brief Mark this input state as pressed input state
   */
  UE_API void MarkAsPressed();

  
  /**
   * @brief Mark this input state as released input state
   */
  UE_API void MarkAsReleased();

private:

  /**
   * @brief Factory to create an instance of input state
   * @return Instance created
   */
  static TPimplPtr<FARInputBufferState> MakeInstanceInternal();

  /**
   * @brief Check input state.It may change state
   */
  void CheckInputStateValidation();

private:

  /**Buffer life time */
  float m_bufferLifeTime;

  /**Input tag */
  FGameplayTag m_inputTag;

  /**Internal state */
  InternalState m_inputState;

};

} // namespace ARRanger::Input

} // namespace ARRanger

#undef UE_API

#endif
