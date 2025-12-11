/**
 * @file ARRangerPlayerController.h
 * @brief Player controller for ARRanger project
 */

#pragma once

#include "GameFramework/PlayerController.h"

#include "GameplayTagContainer.h"

#include "ARRangerPlayerController.generated.h"

/**Forward declaration */
class UInputMappingContext;
class UARInputMappingContext;
class UARAbilitySystemComponent;
class UARPlayerInputBuffer;
class UARInputConfig;
class UARInputComponent;
class UARPlayerPresenter;
class AARRangerCharacter;
struct FGameplayTag;
struct FGameplayTagContainer;
struct FInputActionValue;

#define UE_API ARRANGER_API

/**
 * @brief Handle class for input hold Game ability 
 */
struct FGA_HoldHandle
{
  friend class AARRangerPlayerController;

  UE_API static const FGA_HoldHandle InvalidHandle;

  UE_API bool IsValid() const;

  friend UE_API bool operator==(const FGA_HoldHandle& Lhs, const FGA_HoldHandle& Rhs);
  friend UE_API bool operator!=(const FGA_HoldHandle& Lhs, const FGA_HoldHandle& Rhs);
  
private:
  FGA_HoldHandle();
  
  void GenerateNewHandle();
  
  void Reset();

  int32 m_handleID;

};

/**
 * @brief Hold handle for Blueprint
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct FGABlueprintableHoldHandle
{
  GENERATED_BODY()

  FGA_HoldHandle Handle = FGA_HoldHandle::InvalidHandle;
};

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AARRangerPlayerController : public APlayerController
{
	GENERATED_BODY()

  DECLARE_DELEGATE_RetVal_TwoParams(FGameplayTag, FOnGameplayAbilityHeld, float, const FGameplayTag&);
public:
  // Delegate for Game ablity holding event
  FOnGameplayAbilityHeld OnGameAbilityHeld;

protected:
  struct FHoldSpec
  {
    FGA_HoldHandle Handle = FGA_HoldHandle::InvalidHandle;

    FGameplayTagContainer InputBlockIgnoreTags;

    UE_API bool IsValid() const;

    UE_API bool operator==(const FHoldSpec& Other) const;
    UE_API bool operator!=(const FHoldSpec& Other) const;

  };

public:

  /**Start AActor Interface */
  UE_API virtual void BeginPlay() override;
  UE_API virtual void EndPlay(const EEndPlayReason::Type EndReason) override;
  /**End AActor Interface */

  /**Get ARRanger Ability system component */
  UFUNCTION(BlueprintPure, Category = "ARRanger|PlayerController", meta = (DisplayName = "Get ARAbilitySystemComponent"))
  UE_API UARAbilitySystemComponent* GetARASC() const;

  /**
   * @brief This function will be called if Gameplay ability is activated
   * @param InNextIMCTag Tag of next input mapping context
   */
  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  UE_API void OnGameplayAbilityActivate(const FGameplayTag& InNextIMCTag);
  
  /**
   * @brief This function will be called if Gameplay ability is ended
   * @param bWasCanceled Flag indicates that ability was canceled or not
   */
  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  UE_API void OnGameplayAbilityEnd(bool bWasCanceled);
  
  /**
   * @brief This function will be called if an input hold gameplay ability is activated 
   * @param InActivatedAbilityTag   Tag of activated gameplay ability
   * @param bBlockInputTag          Block input that contains tag if true
   * @param InInputBlockIgnoreTags  Tags to ignore block statement.Only use if bBlockInputTag is true
   * 
   * @return FGABlueprintableHoldHandle Hold handle that can store in blueprint. Invalid handle if bBlockInputTag is false
   */
  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  [[nodiscard]] UE_API FGABlueprintableHoldHandle OnGameplayAbilityActivated_Hold(FGameplayTag InActivatedAbilityTag, bool bBlockInputTag, FGameplayTagContainer InInputBlockIgnoreTags);

  /**
   * @brief This function will be called if an input hold gameplay ability is ticking task
   * @param InTaskOwnerAbilityTag Tag of task owner gameplay ability
   * @param HeldTime Interval time that has been held from beginning
   * @param DeltaTime Time between current and previous TickTask() called 
   */
  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  UE_API void OnGameplayAbilityTaskTicked_Holding(FGameplayTag InTaskOwnerAbilityTag, float HeldTime, float DeltaTime);
  
  /**
   * @brief This function will be called if an input hold gameplay ability is ended 
   * @param InEndedAbilityTag     Tag of ended gameplay ability
   * @param InHandle              Handle that created by OnGameplayAbilityActivated_Hold. @see OnGameplayAbilityActivated_Hold()
   * @param TimeHeld              Total time of an input hold gameplay ability from activating to ending
   */
  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  UE_API void OnGameplayAbilityEnded_Hold(FGameplayTag InEndedAbilityTag, FGABlueprintableHoldHandle InHandle, float TimeHeld);

  UE_API virtual void OnAbilityInputTagPressedEvaluated(FGameplayTag InInputTag);

  UE_API virtual void OnAbilityInputTagReleasedEvaluated(FGameplayTag InInputTag);

protected:
  /**Start APlayerController Interface */
	UE_API virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
  // TODO Temporary
  UE_API virtual void OnPossess(APawn* InPawn) override;
  /**End APlayerController Interface */

  /** Input mapping context setup */
  UE_API virtual void SetupInputComponent() override;

  UE_API FHoldSpec* FindHoldSpecFromHandle(const FGA_HoldHandle& InHoldHandle) const;

  UE_API void ClearHoldSpec(const FGA_HoldHandle& InHoldHandle);

  UE_API bool IsInputBlocked(const FGameplayTag& InInputTag) const;
  
private:

  void SwitchNextIMC(const FGameplayTag& InNextIMCTag);
  void Initialize();
  void InitializePlayerInputBuffer();

  /**Start gameplay ability input callback function */
  void AbilityInputTagPressed(FGameplayTag InInputTag, bool bOverrideInputState = true);
  void AbilityInputTagReleased(FGameplayTag InInputTag, bool bOverrideInputState = true);
  /**End gameplay ability input callback function */

  void EvaluateInputBuffer(const float DeltaTime, const bool bGamePaused);

  /**Start native input callback function */
  void NativeInput_Move(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag);
  void NativeInput_ToggleLockOn(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag);
  void NativeInput_SwitchTarget_Right(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag);
  void NativeInput_SwitchTarget_Left(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag);
  void NativeInput_Transform(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag);
  void NativeInput_ChargeRotate(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag);
  void NativeInput_ResetCamera(const FInputActionValue& InputActionValue, /**PayLoad */ FGameplayTag InInputTag);
  /**End native input callback function */
  
private:

  UPROPERTY(EditDefaultsOnly, Category = "ARInput|InputConfig")
  TSubclassOf<UARPlayerInputBuffer> InputBufferClass;

  UPROPERTY(EditDefaultsOnly, Category = "ARInput|InputConfig")
  TObjectPtr<const UARInputConfig> InputConfig;

  UPROPERTY(EditDefaultsOnly, Category = "ARInput|InputConfig")
  TObjectPtr<const UARInputMappingContext> InputMappingContext;

  UPROPERTY(EditDefaultsOnly, Category = "Presenter")
  TSubclassOf<UARPlayerPresenter> PlayerPresenterClass;

  // 移動時のデッドゾーン(下回ると移動しない)
  UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
  float MoveDeadZone = 0.15f;

  // 移動時インプットの最低値(デッドゾーンを上回っている際の最低値)
  UPROPERTY(EditDefaultsOnly, Category = "Input|Movement")
  float MinInput = 0.3f;

  UPROPERTY(Transient)
  TObjectPtr<UARPlayerInputBuffer> InputBuffer;

  UPROPERTY()
  TObjectPtr<UInputMappingContext> CurrentIMC = nullptr;

  UPROPERTY()
  TObjectPtr<AARRangerCharacter> OwningCharacter;

  UPROPERTY()
  TObjectPtr<UARPlayerPresenter> PlayerPresenter;

  TArray<FHoldSpec> m_holdSpecs;
  
  TArray<uint32> m_bindHandles;
};

#undef UE_API
