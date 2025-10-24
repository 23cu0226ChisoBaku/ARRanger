// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"

#include "ARPlayerPresenter.generated.h"

class AARRangerCharacter;
class UARHealthComponent;

#define UE_API ARRANGER_API

USTRUCT()
struct FARPlayerModel
{
  GENERATED_BODY()

  UPROPERTY()
  TObjectPtr<UARHealthComponent> HealthComponent;

};

UCLASS()
class UARPlayerPresenter : public UObject
{
	GENERATED_BODY()

private:

  UPROPERTY()
  TObjectPtr<AARRangerCharacter> View;
	
  UPROPERTY()
  FARPlayerModel Model;

};

#undef UE_API