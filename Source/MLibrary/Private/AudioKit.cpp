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
    m_audioPlayer.Reset();
  }

  FSoundEffectHandle AudioKit::PlaySE3D(const FString& seName, float Pitch, const FVector& location)
  {
    if (!m_audioPlayer.IsValid())
    {
      return FSoundEffectHandle::InvalidHandle;
    }

    return m_audioPlayer->PlaySE3D(seName, Pitch, location);
  }

  FSoundEffectHandle AudioKit::PlaySE(const FString& seName, float Pitch)
  {
    if (!m_audioPlayer.IsValid())
    {
      return FSoundEffectHandle::InvalidHandle;
    }

    return m_audioPlayer->PlaySE(seName, Pitch);
  }

  bool AudioKit::StopSE(const FSoundEffectHandle& seHandle)
  {
    if (!m_audioPlayer.IsValid())
    {
      return false;
    }

    return m_audioPlayer->StopSE(seHandle);
  }

  void AudioKit::PlayBGM(const FString& bgmName, float Pitch)
  {
    if (!m_audioPlayer.IsValid())
    {
      return;
    }

    m_audioPlayer->PlayBGM(bgmName, Pitch);
  }

  bool AudioKit::StopBGM()
  {
    if (!m_audioPlayer.IsValid())
    {
      return false;
    }

    return m_audioPlayer->StopBGM();
  }
} // namespace MLibrary::UE::Audio

} // namespace MLibrary

