// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioSystem/ARAudioSystem.h"
#include "AudioKit.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARAudioSystem)

void UARAudioSystem::Initialize(FSubsystemCollectionBase& Collection)
{
  Super::Initialize(Collection);

  MAudioKit::AssignAudioPlayer(this);
}

void UARAudioSystem::Deinitialize()
{
  m_seBuffer.Empty();
  m_bgmBuffer.Empty();
  
  if (m_bgmComp != nullptr)
  {
    m_bgmComp->Stop();
    m_bgmComp->DestroyComponent();
    m_bgmComp = nullptr;
  }

  for (const auto& handle : m_seHandles)
  {
    if (handle.IsValid())
    {
      handle->Stop();
      handle->DestroyComponent();
    }
  }
  m_seHandles.Empty();

  MAudioKit::DetachAudioPlayer();

  Super::Deinitialize();
}

FSoundEffectHandle UARAudioSystem::PlaySE3D(const FString& SEName, float Pitch, const FVector& Location)
{
  if (!m_seBuffer.Contains(SEName))
  {
    return FSoundEffectHandle{};
  }

  // 音を3D空間で再生する
  UAudioComponent* resultComp = PlaySE3DImpl(SEName, Pitch, Location);
  if (resultComp == nullptr)
  {
    return FSoundEffectHandle{};
  }

  FSoundEffectHandle resultHandle = FSoundEffectHandle{resultComp};
  m_seHandles.Emplace(FSoundEffectHandle{resultComp});

  return resultHandle;
}

FSoundEffectHandle UARAudioSystem::PlaySE(const FString& SEName, float Pitch)
{
    if (!m_seBuffer.Contains(SEName))
  {
    return FSoundEffectHandle{};
  }

  // 音を再生する
  UAudioComponent* resultComp = PlaySEImpl(SEName, Pitch);
  if (resultComp == nullptr)
  {
    return FSoundEffectHandle{};
  }

  FSoundEffectHandle resultHandle = FSoundEffectHandle{resultComp};
  m_seHandles.Emplace(FSoundEffectHandle{resultComp});

  return resultHandle;
}

bool UARAudioSystem::StopSE(const FSoundEffectHandle& SEHandle)
{
  int32 idx = 0;
  while (idx < m_seHandles.Num())
  {
    const FSoundEffectHandle& handle = m_seHandles[idx];
    if (!handle.IsValid())
    {
      m_seHandles.Remove(handle);
      continue;
    }

    if (IsEqual(SEHandle, handle))
    {
      handle->Stop();
      m_seHandles.Remove(handle);
      return true;
    }

    ++idx;
  }

  return false;
}

void UARAudioSystem::PlayBGM(const FString& BGMName, float Pitch)
{
if (!m_bgmBuffer.Contains(BGMName))
  {
    return;
  }

  USoundBase* se = m_bgmBuffer[BGMName].IsValid() ? m_bgmBuffer[BGMName].Get() : m_bgmBuffer[BGMName].LoadSynchronous();
  if (se != nullptr)
  {
    if (m_bgmComp == nullptr)
    {
      m_bgmComp = UGameplayStatics::CreateSound2D(GetWorld(), se, volume);
      if (m_bgmComp != nullptr)
      {
        m_bgmComp->Play();
      }
    }
    else
    {
      if (m_bgmComp->IsPlaying())
      {
        m_bgmComp->Stop();
      }

      m_bgmComp->SetSound(se);
      m_bgmComp->Play();
    }
  }
}

bool UARAudioSystem::StopBGM()
{

}

void UARAudioSystem::Tick(float DeltaTime)
{

}


void UARAudioSystem::InitializeSounds(UDataTable* bgmTable, UDataTable* seTable)
{

}

UAudioComponent* UARAudioSystem::PlaySE3DImpl(const FString& SEName, float Pitch, const FVector& Location)
{
  USoundBase* soundEffectAsset = m_seBuffer[SEName].IsValid() ? m_seBuffer[SEName].Get() : m_seBuffer[SEName].LoadSynchronous();
  if (soundEffectAsset != nullptr)
  {
    UAudioComponent* soundEffectAudioComp = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), soundEffectAsset, Location, FRotator::ZeroRotator, Pitch);
    if (soundEffectAudioComp != nullptr)
    {
      soundEffectAudioComp->Play();
      return soundEffectAudioComp;
    }
  }

  return nullptr;
}

UAudioComponent* UARAudioSystem::PlaySEImpl(const FString& SEName, float Pitch)
{
  USoundBase* soundEffectAsset = m_seBuffer[SEName].IsValid() ? m_seBuffer[SEName].Get() : m_seBuffer[SEName].LoadSynchronous();
  if (soundEffectAsset != nullptr)
  {
    UAudioComponent* soundEffectAudioComp = UGameplayStatics::CreateSound2D(GetWorld(), soundEffectAsset, Pitch);
    if (soundEffectAudioComp != nullptr)
    {
      soundEffectAudioComp->Play();
      return soundEffectAudioComp;
    }
  }

  return nullptr;
}

UAudioComponent* UARAudioSystem::PlayBGMImpl(const FString& SEName, float Pitch)
{
  
}