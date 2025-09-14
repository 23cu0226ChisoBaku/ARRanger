// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"

#include "GameplayTagContainer.h"

#include "ARRangerPlayerController.generated.h"

class UInputMappingContext;
class UARInputMappingContext;
class UARAbilitySystemComponent;
class UARPlayerInputBuffer;
class UARInputConfig;
class UARInputComponent;
struct FGameplayTag;
struct FGameplayTagContainer;

#define UE_API ARRANGER_API

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameplayAbilityHeld, float);

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

public:

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

  UFUNCTION(BlueprintPure, Category = "ARRanger|PlayerController", meta = (DisplayName = "Get ARAbilitySystemComponent"))
  UE_API UARAbilitySystemComponent* GetARASC() const;

  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  UE_API void OnGameplayAbilityActivate(const FGameplayTag& InNextIMCTag);
  
  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  UE_API void OnGameplayAbilityEnd(bool bWasCanceled);
  
  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  [[nodiscard]] UE_API FGABlueprintableHoldHandle OnGameplayAbilityActivated_Hold(bool bBlockInputTag, FGameplayTagContainer InInputBlockIgnoreTags);
  
  UFUNCTION(BlueprintCallable, Category = "ARRanger|PlayerController")
  UE_API void OnGameplayAbilityEnded_Hold(FGABlueprintableHoldHandle InHandle, float TimeHeld);
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

private:

  void SwitchNextIMC(const FGameplayTag& InNextIMCTag);

  void InitializePlayerInput();

  void InitializePlayerInputBuffer(UARInputComponent* InInputComponent);

  void AbilityInputTagPressed(FGameplayTag InInputTag);
  void AbilityInputTagReleased(FGameplayTag InInputTag);
  void EvaluateInputBuffer(const float DeltaTime, const bool bGamePaused);

  bool IsInputBlocked(const FGameplayTag& InInputTag) const;
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

  TArray<FHoldSpec> m_holdSpecs;
  
  TArray<uint32> m_bindHandles;
};

#undef UE_API
