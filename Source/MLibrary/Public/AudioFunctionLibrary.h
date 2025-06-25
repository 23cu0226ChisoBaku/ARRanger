// Fill out your copyright notice in the Description page of Project Settings.
/*

AudioFunctionLibrary.h

Author : MAI ZHICONG(バクチソウ)

Description : 簡単Audio再生BPライブラリ

Update History: 2025/02/08  作成

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoundEffectHandle.h"
#include "AudioFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MLIBRARY_API UAudioFunctionLibrary : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()

public:
  ///
  /// @brief            SEを3D空間で再生
  /// @param seName     再生するSEの名前
  /// @param volume     再生する音量（PitchMultiplier）
  /// @param location   再生する座標
  ///
  UFUNCTION(BlueprintCallable, Category = "Audio Kit")
  static void PlaySE3D(const FString& seName, float volume = 1.0f, const FVector& location = FVector::ZeroVector);
  ///
  /// @brief            SEを再生
  /// @param seName     再生するSEの名前
  /// @param volume     再生する音量（PitchMultiplier）
  ///
  UFUNCTION(BlueprintCallable, Category = "Audio Kit")
  static void PlaySE(const FString& seName, float volume = 1.0f);
  ///
  /// @brief            BGMを再生
  /// @param bgmName    再生するBGMの名前
  /// @param volume     再生する音量（PitchMultiplier）
  ///
  UFUNCTION(BlueprintCallable, Category = "Audio Kit")
  static void PlayBGM(const FString& bgmName, float volume = 1.0f);
  ///
  /// @brief            再生するBGMを停止
  ///
  UFUNCTION(BlueprintCallable, Category = "Audio Kit")
  static void StopBGM();
  
};
