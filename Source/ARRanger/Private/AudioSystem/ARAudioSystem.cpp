// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioSystem/ARAudioSystem.h"
#include "AudioKit.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARAudioSystem)

namespace
{
  void RemoveInvalidHandles(TArray<FSoundEffectHandle>& OriginHandles, const TArray<const FSoundEffectHandle*>& RemoveHandles);
}

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
  auto searchHandleToRemove = [&SEHandle](const FSoundEffectHandle& Handle)
  {
    return SEHandle == Handle;
  };

  FSoundEffectHandle* foundHandle = m_seHandles.FindByPredicate(searchHandleToRemove);
  if (foundHandle != nullptr)
  {
    (*foundHandle)->Stop();
    (*foundHandle)->DestroyComponent();
    m_seHandles.Remove(*foundHandle);
    return true;
  }

  return false;
}

void UARAudioSystem::PlayBGM(const FString& BGMName, float Pitch)
{
  if (!m_bgmBuffer.Contains(BGMName))
  {
    return;
  }

  PlayBGMImpl(BGMName, Pitch);
}

bool UARAudioSystem::StopBGM()
{
  if (!::IsValid(m_bgmComp))
  {
    return false;
  }

  m_bgmComp->Stop();
  m_bgmComp->SetSound(nullptr);
  return true;
}

void UARAudioSystem::Tick(float DeltaTime)
{
  for (const auto& handle : m_seHandles)
  {
    // 無効なハンドラあるいは再生が終了したSEを削除予定リストに入れる
    if (!handle.IsValid() || !handle->IsPlaying())
    {
      m_requestRemoveHandles.Emplace(&handle);
    }
  }

  // 無効なSEハンドルを全部削除する
  if (m_requestRemoveHandles.Num() > 0)
  {
    RemoveInvalidHandles(m_seHandles, m_requestRemoveHandles);
    m_requestRemoveHandles.Reset();
  }
}


void UARAudioSystem::InitializeSounds(UDataTable* bgmTable, UDataTable* seTable)
{
  TArray<FARSoundMetaData*> audioSources;
  if (bgmTable != nullptr)
  {
    bgmTable->GetAllRows<FARSoundMetaData>(nullptr, audioSources);
    for (const auto& audioSource : audioSources)
    {
      if (!m_bgmBuffer.Contains(audioSource->SoundID))
      {
        m_bgmBuffer.Emplace(audioSource->SoundID, audioSource->SoundAsset);
      }
    }
  }

  audioSources.Reset();
  if (seTable != nullptr)
  {
    seTable->GetAllRows<FARSoundMetaData>(nullptr, audioSources);
    for (const auto& audioSource : audioSources)
    {
      if (!m_seBuffer.Contains(audioSource->SoundID))
      {
        m_seBuffer.Emplace(audioSource->SoundID, audioSource->SoundAsset);
      }
    }   
  }
}

UAudioComponent* UARAudioSystem::PlaySE3DImpl(const FString& SEName, float Pitch, const FVector& Location)
{
  USoundBase* soundEffectAsset = m_seBuffer[SEName].IsValid() ? m_seBuffer[SEName].Get() : m_seBuffer[SEName].LoadSynchronous();
  if (soundEffectAsset != nullptr)
  {
    UAudioComponent* soundEffectAudioComp = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), soundEffectAsset, Location, FRotator::ZeroRotator, 1.0f, Pitch);
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
    UAudioComponent* soundEffectAudioComp = UGameplayStatics::CreateSound2D(GetWorld(), soundEffectAsset, 1.0f, Pitch);
    if (soundEffectAudioComp != nullptr)
    {
      soundEffectAudioComp->Play();
      return soundEffectAudioComp;
    }
  }

  return nullptr;
}

void UARAudioSystem::PlayBGMImpl(const FString& BGMName, float Pitch)
{
  USoundBase* bgmAsset = m_bgmBuffer[BGMName].IsValid() ? m_bgmBuffer[BGMName].Get() : m_bgmBuffer[BGMName].LoadSynchronous();
  if (bgmAsset != nullptr)
  {
    if (m_bgmComp == nullptr)
    {
      m_bgmComp = UGameplayStatics::CreateSound2D(this, bgmAsset, 1.0f, Pitch);
      if (m_bgmComp != nullptr)
      {
        m_bgmComp->Play();
      }
    }
    else
    {
      SwitchBGM(bgmAsset);
    }
  }
}

void UARAudioSystem::SwitchBGM(USoundBase* NewBGMAsset)
{
  check(m_bgmComp != nullptr);
  if (NewBGMAsset == nullptr)
  {
    return;
  }

  if (m_bgmComp->IsPlaying())
  {
    m_bgmComp->Stop();
  }

  m_bgmComp->SetSound(NewBGMAsset);
  m_bgmComp->Play();
}

namespace
{
  void RemoveInvalidHandles(TArray<FSoundEffectHandle>& OriginHandles, const TArray<const FSoundEffectHandle*>& RemoveHandles)
  {
    for (int32 i = 0; i < RemoveHandles.Num(); ++i)
    {
      const FSoundEffectHandle* removeHandlePtr = RemoveHandles[i];
      if (removeHandlePtr == nullptr)
      {
        continue;
      }

      auto pred = [&removeHandlePtr](const FSoundEffectHandle& Handle)
      {
        return Handle == *removeHandlePtr;
      };

      FSoundEffectHandle* foundHandlePtr = OriginHandles.FindByPredicate(pred);
      if (foundHandlePtr != nullptr)
      {
        OriginHandles.RemoveSingle(*foundHandlePtr);
      }
    }
  }
}