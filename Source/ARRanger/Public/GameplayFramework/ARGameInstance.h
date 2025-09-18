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
    UE_API void ResetBattleTestStage();

  public:
    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UMagneticParametersList> MagneticParamsAsset;

    UPROPERTY(EditDefaultsOnly)
    FName ResetGameLevelName;

    UPROPERTY(EditDefaultsOnly)
    FName ResetBattleTestStageLevelName;

    // For debug purpose
    FSimpleMulticastDelegate OnReset;
};

#undef UE_API
