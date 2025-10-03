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
class UARAbilitySystemComponent;

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
   * @param InPlayerController PlayerController using by ARRanger project
   */
  UE_API void Initialize(AARRangerPlayerController* InPlayerController);

  /**
   * @brief Uninitialize input buffer
   */
  UE_API void Uninitialize();

  /**
   * @brief Evaluate input buffer (Tick base)
   * @param InPlayerController Player controller using by ARRanger project
   * @param DeltaTime DeltaTime(Frame rate base)
   * @param bGamePaused Flag indicating whether the game is paused 
   */
  UE_API void EvaluateBuffer(const float DeltaTime, const bool bGamePaused);

  /**
   * @brief Clear all buffers
   */
  UE_API void ClearAllInputs();

  /**
   * @brief Handle input tag when button pressed
   * @param InInputTag input signature tag payload that binds to input action
   */
  UE_API void HandleInputTagPressed(const FGameplayTag& InInputTag, bool bOverrideInputState);

  /**
   * @brief Handle input tag when button released
   * @param InInputTag input signature tag payload that binds to input action
   */
  UE_API void HandleInputTagReleased(const FGameplayTag& InInputTag, bool bOverrideInputState);

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
  void RemoveInputBufferInternal();

  /**
   * @brief Handle ability activation result
   * @param InAbilitySystemComponent Ability system component using in project ARRanger
   * @param InAbilityAssetTags       Tags of ability that try to activate
   * @param bResult                  Result of ability activation
   */
  UFUNCTION()
  void OnTryActivateAbilityHandled(UARAbilitySystemComponent* InAbilitySystemComponent, FGameplayTagContainer InAbilityAssetTags, bool bResult);

private:

  /**Buffer valid time when an input pressed */
  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Input", meta = (AllowPrivateAccess = "true"))
  float InputKeepTime = 0.0f;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Input", meta = (AllowPrivateAccess = "true"))
  bool bEvaluateIfGamePaused = false;

  UPROPERTY()
  TObjectPtr<AARRangerPlayerController> PlayerController;

  TArray< TPimplPtr< ARRanger::Input::FARInputBufferState > > m_inputStates;

};

#undef UE_API