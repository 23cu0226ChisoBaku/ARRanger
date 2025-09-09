// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ARInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FARInputAction
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  TObjectPtr<const UInputAction> InputAction = nullptr;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  FGameplayTag InputTag = FGameplayTag::EmptyTag;
};


UCLASS(BlueprintType, Const)
class ARRANGER_API UARInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

  UARInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  UFUNCTION(BlueprintCallable, Category = "InputConfig")
  const UInputAction* FindAbilityInputAction(const FGameplayTag& InInputTag) const;

  UPROPERTY(EditDefaultsOnly, Category = "InputConfig", meta = (TitleProperty = "AbilityAction"))
  TArray<FARInputAction> AbilityInputActions;
};
