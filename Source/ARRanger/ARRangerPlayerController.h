// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"

#include "ARRangerPlayerController.generated.h"

class UInputMappingContext;
class UARInputMappingContext;
class UARAbilitySystemComponent;
class UARPlayerInputBuffer;
class UARInputConfig;
class UARInputComponent;
struct FGameplayTag;

#define UE_API ARRANGER_API

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AARRangerPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

  UFUNCTION(BlueprintPure, Category = "ARRanger|PlayerController", meta = (DisplayName = "Get ARAbilitySystemComponent"))
  UE_API UARAbilitySystemComponent* GetARASC() const;

  /**Start APlayerController Interface */
	UE_API virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
protected:
  // TODO Temporary
  UE_API virtual void OnPossess(APawn* InPawn) override;
  /**End APlayerController Interface */
	
protected:

  /** Input mapping context setup */
  UE_API virtual void SetupInputComponent() override;

private:

  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  UE_API void OnGameplayAbilityActivate(const FGameplayTag& InNextIMCTag);

  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  UE_API void OnGameplayAbilityEnd(bool bWasCanceled);

private:

  void SwitchNextIMC(const FGameplayTag& InNextIMCTag);

  void InitializePlayerInput();

  void InitializePlayerInputBuffer(UARInputComponent* InInputComponent);

  void AbilityInputTagPressed(FGameplayTag InInputTag);
  void AbilityInputTagReleased(FGameplayTag InInputTag);
  void EvaluateInputBuffer(const float DeltaTime, const bool bGamePaused);

private:

  UPROPERTY(EditDefaultsOnly, Category = "ARInput|InputConfig")
  TSubclassOf<UARPlayerInputBuffer> InputBufferClass;

  UPROPERTY(EditDefaultsOnly, Category = "ARInput|InputConfig")
  TObjectPtr<const UARInputConfig> InputConfig;

  UPROPERTY(EditDefaultsOnly, Category = "ARInput|InputConfig")
  TObjectPtr<const UARInputMappingContext> InputMappingContext;

  UPROPERTY(Transient)
  TObjectPtr<UARPlayerInputBuffer> InputBuffer;

  UPROPERTY()
  TObjectPtr<UInputMappingContext> CurrentIMC = nullptr;
  
  TArray<uint32> m_bindHandles;
};

#undef UE_API
