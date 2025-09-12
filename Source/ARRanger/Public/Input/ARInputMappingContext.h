// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "ARInputMappingContext.generated.h"

#define UE_API ARRANGER_API

class UInputMappingContext;

USTRUCT(BlueprintType)
struct FARInputMappingContextAndTag
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly)
  TSoftObjectPtr<UInputMappingContext> InputMapping;

  UPROPERTY(EditDefaultsOnly)
  FGameplayTag InputStateTag;

};

/**
 * 
 */
UCLASS(Const)
class UARInputMappingContext : public UDataAsset
{
	GENERATED_BODY()

public:

  UE_API UInputMappingContext* FindIMCWithTag(const FGameplayTag& InTag) const;
  
  UPROPERTY(EditDefaultsOnly, Category = "InputMappingContext", meta = (TitleProperty = "{InputMapping} with state tag: {InputStateTag}"))
  TArray<FARInputMappingContextAndTag> InputMappingContexts;
};

#undef UE_API
