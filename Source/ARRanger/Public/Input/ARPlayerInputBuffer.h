/**
 *  ARPlayerInputBuffer.h
 */

#pragma once

#include "UObject/NoExportTypes.h"

#include "GameplayTagContainer.h"

#include "ARPlayerInputBuffer.generated.h"

#define UE_API ARRANGER_API

/**Forward declaration */
class AARRangerPlayerController;
class UARInputComponent;

namespace ARRanger
{

namespace Input
{
  class FARInputBufferState;
} // namespace ARRanger::Input

} // namespace ARRanger

/**
 * @brief Input buffer for ARRanger project use
 */
UCLASS(Blueprintable, BlueprintType)
class UARPlayerInputBuffer : public UObject
{
	GENERATED_BODY()

public:

  /**Default constructor */
  UE_API UARPlayerInputBuffer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  /**
   * @brief Initialize input buffer
   * @param InInputComponent UARInputComponent (using by ARRanger project)
   */
  UE_API void InitializeInputBuffer(UARInputComponent* InInputComponent);

  /**
   * @brief Uninitialize input buffer
   */
  UE_API void UninitializeInputBuffer();

  /**
   * @brief Evaluate input buffer (Tick base)
   * @param InPlayerController Player controller using by ARRanger project
   * @param DeltaTime DeltaTime(Frame rate base)
   * @param bGamePaused Flag indicating whether the game is paused 
   */
  UE_API void EvaluateBuffer(const AARRangerPlayerController* InPlayerController, const float DeltaTime, const bool bGamePaused);

  /**
   * @brief Check if input buffer exist
   * @param InInputTag input tag (input signature)
   * 
   * @return True if buffer is exist and valid, otherwise false
   */
  UE_API bool IsInputBufferValid(const FGameplayTag& InInputTag) const;

  /**
   * @brief Clear all buffers
   */
  UE_API void ClearAllInputs();

  /**
   * @brief Handle input tag when button pressed
   * @param InInputTag input signature tag payload that binds to input action
   */
  UE_API void HandleInputTagPressed(const FGameplayTag& InInputTag);

  /**
   * @brief Handle input tag when button released
   * @param InInputTag input signature tag payload that binds to input action
   */
  UE_API void HandleInputTagReleased(const FGameplayTag& InInputTag);

private:

  /**
   * @brief Consume input buffer
   * @param InInputTag signature input tag
   */
  void ConsumeBuffer(const FGameplayTag& InInputTag);

  /**
   * @brief Remove input buffer
   * @param InPlayerController Player controller using by ARRanger project
   */
  void RemoveInputBufferInternal(const AARRangerPlayerController* InPlayerController);

private:

  /**Buffer valid time when an input pressed */
  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Input", meta = (AllowPrivateAccess = "true"))
  float InputKeepTime = 0.0f;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Input", meta = (AllowPrivateAccess = "true"))
  bool bEvaluateIfGamePaused = false;

  UPROPERTY()
  TObjectPtr<UARInputComponent> InputComponent;

  /**List of buffer with signature tag(Key) and life time(Value)*/
  TMap<FGameplayTag, float> m_inputTagBuffers;

  /**Array of input signature tag to remove in next evaluation call*/
  TArray<FGameplayTag> m_removeTags;

  TArray< TPimplPtr< ARRanger::Input::FARInputBufferState > > m_inputStates;

};

#undef UE_API