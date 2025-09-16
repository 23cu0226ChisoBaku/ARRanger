// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "ARPawnInitData.generated.h"

#define UE_API ARRANGER_API

class UARGameplayAbilityBase;
class UARAttributeSet;

/**
 * 
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Pawn Init Data"))
class UARPawnInitData : public UDataAsset
{
	GENERATED_BODY()
	
public:
  UE_API UARPawnInitData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayAbilities")
  TSet< TSoftClassPtr< UARGameplayAbilityBase > > Abilities;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayAbilities")
  TSet< TSoftClassPtr<UARAttributeSet> > AttributeSets;

};

#undef UE_API