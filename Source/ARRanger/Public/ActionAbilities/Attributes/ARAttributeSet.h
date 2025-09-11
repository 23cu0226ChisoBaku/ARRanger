// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AttributeSet.h"

#include "ARAttributeSet.generated.h"

class UARAbilitySystemComponent;

#define UE_API ARRANGER_API

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
  GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
  GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
  GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
  GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class UARAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

 UE_API UARAttributeSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

 UE_API UWorld* GetWorld() const override;

 UE_API UARAbilitySystemComponent* GetARAbilitySystemComponent() const;
};

#undef UE_API
