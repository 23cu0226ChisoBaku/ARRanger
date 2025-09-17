// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"

#include "ARGameInstance.generated.h"

class UMagneticParametersList;

#define UE_API ARRANGER_API

UCLASS()
class UARGameInstance : public UGameInstance
{
	GENERATED_BODY()

  public:
    UE_API void ResetGame();

  public:
    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UMagneticParametersList> MagneticParamsAsset;

    UPROPERTY(EditDefaultsOnly)
    FName ResetGameLevelName;
};

#undef UE_API
