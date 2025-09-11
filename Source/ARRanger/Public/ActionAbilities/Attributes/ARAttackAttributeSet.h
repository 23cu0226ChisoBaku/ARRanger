// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActionAbilities/Attributes/ARAttributeSet.h"
#include "AbilitySystemComponent.h"

#include "ARAttackAttributeSet.generated.h"

#define UE_API ARRANGER_API

/**
 * 
 */
UCLASS(BlueprintType)
class UARAttackAttributeSet : public UARAttributeSet
{
	GENERATED_BODY()

public:

  UE_API UARAttackAttributeSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  ATTRIBUTE_ACCESSORS(UARAttackAttributeSet, BaseDamage);

private:

  UPROPERTY(BlueprintReadOnly, Category = "Attribute|Attack", meta = (HideFromModifiers, AllowPrivateAccess = "true"))
	FGameplayAttributeData BaseDamage;
};

#undef UE_API