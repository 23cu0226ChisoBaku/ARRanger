// Fill out your copyright notice in the Description page of Project Settings.
/*

AudioKit.h

Author : MAI ZHICONG

Description : オーディオ再生ツール

Update History: 2025/02/08 作成

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef MLIBRARY_AUDIO_KIT
#define MLIBRARY_AUDIO_KIT

#include "AudioSystem/SoundEffectHandle.h"
#include "UObject/WeakInterfacePtr.h"

#define UE_API ARRANGER_API

class IAudioPlayer;

namespace MLibrary
{
namespace UE::Audio
{
  class AudioKit
  {
    public:

      /**
       * @brief              オーディオプレイヤーアタッチする
       * @param IAudioPlayer オーディオプレイヤーインターフェース
       */
      UE_API static void AssignAudioPlayer(IAudioPlayer* AudioPlayerPtr);
      UE_API static void DetachAudioPlayer();
      ///
      /// @brief              サウンドエフェクトを3D空間で再生
      /// @param seName       サウンドエフェクトの名前
      /// @param volume       ボリューム
      /// @param location     再生座標
      /// @return             再生するエフェクトハンドル
      ///
      UE_API static FSoundEffectHandle PlaySE3D(const FString& seName, float pitch = 1.0f, const FVector& location = FVector::ZeroVector);
      ///
      /// @brief              サウンドエフェクトを再生 
      /// @param seName       サウンドエフェクトの名前
      /// @param volume       ボリューム
      /// @return             再生するエフェクトハンドル
      /// 
      UE_API static FSoundEffectHandle PlaySE(const FString& seName, float pitch = 1.0f);
      ///
      /// @brief              サウンドエフェクトの再生を中止
      /// @param              エフェクトハンドル
      /// @return             再生中止成功  ->  true
      ///                     再生中止失敗  ->  false
      ///
      UE_API static bool StopSE(const FSoundEffectHandle& seHandle);
      ///
      /// @brief              BGMを再生     
      /// @param bgmName      BGM名前
      /// @param volume       ボリューム
      ///
      UE_API static void PlayBGM(const FString& bgmName, float volume = 1.0f);
      ///
      /// @brief              BGMの再生を中止  
      /// @return             再生中止成功  ->  true
      ///                     再生中止失敗  ->  false
      ///
      UE_API static bool StopBGM();

    private:
      // オーディオプレイヤーインターフェース
      static TWeakInterfacePtr<IAudioPlayer> m_audioPlayer;

    // インスタンス生成禁止
    public:
      AudioKit() = delete;
      ~AudioKit() = delete;
  };
}
}

using MAudioKit = MLibrary::UE::Audio::AudioKit; 

#undef UE_API

#endif
