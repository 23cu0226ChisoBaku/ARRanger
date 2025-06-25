// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioKit.h"
#include "IAudioPlayer.h"

namespace MLibrary
{
	namespace UE::Audio
	{
    TWeakInterfacePtr<IAudioPlayer> AudioKit::m_audioPlayer = nullptr;

    void AudioKit::AssignAudioPlayer(IAudioPlayer* newAudioPlayer)
    {
      m_audioPlayer = newAudioPlayer;
    }
    void AudioKit::DetachAudioPlayer()
    {
      m_audioPlayer = nullptr;
    }
    FSoundEffectHandle AudioKit::PlaySE3D(const FString& seName, float volume, const FVector& location)
    {
      if (!m_audioPlayer.IsValid())
      {
        return {};
      }

      return m_audioPlayer->PlaySE3D(seName, volume, location);
    }
    FSoundEffectHandle AudioKit::PlaySE(const FString& seName, float volume)
    {
      if (!m_audioPlayer.IsValid())
      {
        return {};
      }

      return m_audioPlayer->PlaySE(seName, volume);
    }
    bool AudioKit::StopSE(const FSoundEffectHandle& seHandle)
    {
      if (!m_audioPlayer.IsValid())
      {
        return false;
      }

      return m_audioPlayer->StopSE(seHandle);
    }
    void AudioKit::PlayBGM(const FString& bgmName, float volume)
    {
      if (!m_audioPlayer.IsValid())
      {
        return;
      }

      m_audioPlayer->PlayBGM(bgmName, volume);
    }
    bool AudioKit::StopBGM()
    {
      if (!m_audioPlayer.IsValid())
      {
        return false;
      }

      return m_audioPlayer->StopBGM();
    }
  }
}

