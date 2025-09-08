// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"

#include "ARGameplayAbilityBase.generated.h"

#define UE_API ARRANGER_API

/**
 * 
 */
UCLASS(Abstract)
class UARGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
  UARGameplayAbilityBase();

  void SetAbilityCancelable();
  
  void SetAbilityBlock();

  bool GetAbilityCancelable() const { return bCanCancel; }

private:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  bool bCanCancel;
	
};

#undef UE_API
