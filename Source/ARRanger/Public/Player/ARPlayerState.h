// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"

#include "ARPlayerState.generated.h"

#define UE_API ARRANGER_API

class UARAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class AARPlayerState : public APlayerState , public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
  UE_API AARPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  /**Start IAbilitySystemInterface Interface */
  UE_API virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override; 
  /**End IAbilitySystemInterface Interface */

  /**Start AActor Interface */
  UE_API virtual void PostInitializeComponents() override;
  /**End AActor Interface */

  UFUNCTION(BlueprintPure, Category = "ARRanger|PlayerState")
  UARAbilitySystemComponent* GetARAbilitySystemComponent() const { return AbilitySystemComponent; }
	
private:

  UPROPERTY(VisibleAnywhere, Category = "GameplayAbility")
  TObjectPtr<UARAbilitySystemComponent> AbilitySystemComponent;

};

#undef UE_API