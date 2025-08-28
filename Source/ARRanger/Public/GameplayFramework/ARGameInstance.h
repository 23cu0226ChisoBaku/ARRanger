// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"

#include "ARGameInstance.generated.h"

class UMagneticParametersList;

UCLASS()
class ARRANGER_API UARGameInstance : public UGameInstance
{
	GENERATED_BODY()

  public:
    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UMagneticParametersList> MagneticParamsAsset;
};
