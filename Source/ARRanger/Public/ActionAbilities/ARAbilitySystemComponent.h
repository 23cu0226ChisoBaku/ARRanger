// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"

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
  // UE_API void NotifyAbilityCancelable();	
};

#undef UE_API