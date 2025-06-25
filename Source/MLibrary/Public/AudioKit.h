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

#include "CoreMinimal.h"
#include "SoundEffectHandle.h"
#include "UObject/WeakInterfacePtr.h"

class IAudioPlayer;

namespace MLibrary
{
  namespace UE::Audio
  {
    class MLIBRARY_API AudioKit
    {
      // インスタンス生成禁止
      private:
        AudioKit() = delete;
        ~AudioKit() = delete;

      public:
        ///
        /// @brief              オーディオプレイヤーアタッチする
        /// @param IAudioPlayer オーディオプレイヤーインターフェース
        ///
        static void AssignAudioPlayer(IAudioPlayer*);
        static void DetachAudioPlayer();
        ///
        /// @brief              サウンドエフェクトを3D空間で再生
        /// @param seName       サウンドエフェクトの名前
        /// @param volume       ボリューム
        /// @param location     再生座標
        /// @return             再生するエフェクトハンドル
        ///
        static FSoundEffectHandle PlaySE3D(const FString& seName, float volume = 1.0f, const FVector& location = FVector::ZeroVector);
        ///
        /// @brief              サウンドエフェクトを再生 
        /// @param seName       サウンドエフェクトの名前
        /// @param volume       ボリューム
        /// @return             再生するエフェクトハンドル
        /// 
        static FSoundEffectHandle PlaySE(const FString& seName, float volume = 1.0f);
        ///
        /// @brief              サウンドエフェクトの再生を中止
        /// @param              エフェクトハンドル
        /// @return             再生中止成功  ->  true
        ///                     再生中止失敗  ->  false
        ///
        static bool StopSE(const FSoundEffectHandle& seHandle);
        ///
        /// @brief              BGMを再生     
        /// @param bgmName      BGM名前
        /// @param volume       ボリューム
        ///
        static void PlayBGM(const FString& bgmName, float volume = 1.0f);
        ///
        /// @brief              BGMの再生を中止  
        /// @return             再生中止成功  ->  true
        ///                     再生中止失敗  ->  false
        ///
        static bool StopBGM();

      private:
        // オーディオプレイヤーインターフェース
        static TWeakInterfacePtr<IAudioPlayer> m_audioPlayer;
    };
  }
}

#endif
