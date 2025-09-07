// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "ARRangerPlayerController.generated.h"

class UInputMappingContext;
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
  /**End APlayerController Interface */
	
protected:

  /** Input mapping context setup */
  UE_API virtual void SetupInputComponent() override;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="Input", meta = (AllowPrivateAccess = "true"))
	TArray<UInputMappingContext*> DefaultMappingContexts;

private:

  void InitializePlayerInput();

  void InitializePlayerInputBuffer(UARInputComponent* InInputComponent);

  void AbilityInputTagPressed(FGameplayTag InInputTag);
  void AbilityInputTagReleased(FGameplayTag InInputTag);
  void EvaluateInputBuffer(const float DeltaTime, const bool bGamePaused);

private:

  UPROPERTY(EditDefaultsOnly, Category = "ARInput|InputConfig")
  TSubclassOf<UARPlayerInputBuffer> InputBufferClass;

  UPROPERTY(EditDefaultsOnly, Category = "ARInput|InputConfig", meta = (AllowPrivateAccess = "true"))
  TObjectPtr<const UARInputConfig> InputConfig;

  UPROPERTY(Transient)
  TObjectPtr<UARPlayerInputBuffer> InputBuffer;

  TArray<uint32> m_bindHandles;

};

#undef UE_API
