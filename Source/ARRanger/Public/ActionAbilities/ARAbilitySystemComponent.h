// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"

#include <type_traits>

#include "ARAbilitySystemComponent.generated.h"

#define UE_API ARRANGER_API


/**
 * 
 */
UCLASS()
class UARAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

  /**
   * @brief 現在のアビリティがキャンセルできることを通知する
   */
  UE_API void NotifyAbilityCancelable();

  /**
   * @brief 現在のアビリティキャンセルをブロックすることを通知する
   */
  UE_API void NotifyAbilityBlock();


  UE_API static UARAbilitySystemComponent* FindARAbilitySystemComponent(AActor* InActor);

private:
  UE_API static UAbilitySystemComponent* FindAbilitySystemComponentImpl(AActor* InActor);

};



#undef UE_API