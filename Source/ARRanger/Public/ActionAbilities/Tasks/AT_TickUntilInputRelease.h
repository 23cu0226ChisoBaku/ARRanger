// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

#include "AT_TickUntilInputRelease.generated.h"

#define UE_API ARRANGER_API

/**
 * 
 */
UCLASS()
class UAT_TickUntilInputRelease : public UAbilityTask_WaitInputRelease
{
	GENERATED_BODY()
  
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityTaskTicked, float, DeltaTime);
  
  /**Start UGameplayTask Interface */
  UE_API virtual void Activate() override;
  UE_API virtual void TickTask(float DeltaTime) override;
  /**End UGameplayTask Interface */

public:
  UE_API UAT_TickUntilInputRelease(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  UPROPERTY(BlueprintAssignable)
  FOnAbilityTaskTicked OnTick;

  UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
  static UE_API UAT_TickUntilInputRelease* TickUntilInputRelease(UGameplayAbility* OwningAbility);

};

#undef UE_API