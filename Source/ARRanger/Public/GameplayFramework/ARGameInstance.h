// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"

#include "ARGameInstance.generated.h"

class UMagneticParametersList;

#define UE_API ARRANGER_API

#if !WITH_EDITOR
#define AR_DEMO_BUILD 1
#endif

#ifndef AR_DEMO_BUILD
#define AR_DEMO_BUILD 0
#endif

UCLASS()
class UARGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorBlindModeEnabledDelegate, bool, bEnable);
  UPROPERTY(BlueprintAssignable)
  FOnColorBlindModeEnabledDelegate OnColorBlindModeEnabled;

  public:
    UE_API void SetColorBlindMode(bool bEnabled);
    UE_API bool IsColorBlindMode() const;
    UE_API void ResetGame();
    UE_API void ResetBattleTestStage();
    UE_API void ProcessGameClear();
    UE_API void ProcessGameOver();

  public:
    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UMagneticParametersList> MagneticParamsAsset;

    UPROPERTY(EditDefaultsOnly)
    FName ResetGameLevelName;

    UPROPERTY(EditDefaultsOnly)
    FName ResetBattleTestStageLevelName;

    UPROPERTY(EditDefaultsOnly)
    FName GameClearLevelName;

    UPROPERTY(EditDefaultsOnly)
    FName GameOverLevelName;

    // For debug purpose
    FSimpleMulticastDelegate OnReset;

    // コンボカウントを保存
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int ComboCount = 0;

  private:
    uint8 bColorBlindModeOn : 1 = false;
};

#undef UE_API
