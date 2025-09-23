// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"

#include "ARPlayerState.generated.h"

#define UE_API ARRANGER_API

class UARAbilitySystemComponent;
class UARChargeAttackComponent;
class AARRangerPlayerController;

/**
 * 
 */
UCLASS()
class AARPlayerState : public APlayerState , public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
  UE_API AARPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  // TODO
  UE_API void OnPlayerStateInitialized(AARRangerPlayerController* InPlayerController);

  /**Start IAbilitySystemInterface Interface */
  UE_API virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override; 
  /**End IAbilitySystemInterface Interface */

  /**Start AActor Interface */
  UE_API virtual void PostInitializeComponents() override;
  UE_API virtual void BeginPlay() override;
  /**End AActor Interface */

  UFUNCTION(BlueprintPure, Category = "ARRanger|PlayerState")
  UARAbilitySystemComponent* GetARAbilitySystemComponent() const { return AbilitySystemComponent; }

  UFUNCTION(BlueprintPure, Category = "ARRanger|PlayerState")
  UARChargeAttackComponent* GetARChargeAttackComponent() const { return ChargeAttackComponent; }
	
private:

  UPROPERTY(VisibleAnywhere, Category = "GameplayAbility")
  TObjectPtr<UARAbilitySystemComponent> AbilitySystemComponent;

  UPROPERTY(VisibleAnywhere, Category = "GameplayAbility")
  TObjectPtr<UARChargeAttackComponent> ChargeAttackComponent;

};

#undef UE_API