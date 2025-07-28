// Fill out your copyright notice in the Description page of Project Settings.
/*

SoundEffectHandle.cpp

Author : MAI ZHICONG

Description : SEハンドル

Update History: 2025/02/07 作成

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include "SoundEffectHandle.h"
#include "Components/AudioComponent.h"

#include <atomic>

namespace
{
  std::atomic<int64> gHandleID = 0;

  constexpr int64 INVALID_HANDLE_ID = -1;
}

const FSoundEffectHandle FSoundEffectHandle::InvalidHandle = FSoundEffectHandle{}; 

FSoundEffectHandle::FSoundEffectHandle()
  : m_uniqueID{INVALID_HANDLE_ID}
  , m_audioComponent{nullptr}
{}

FSoundEffectHandle::FSoundEffectHandle(UAudioComponent* audioComp)
  : m_uniqueID{INVALID_HANDLE_ID}
  , m_audioComponent{audioComp}
{
  if (audioComp != nullptr)
  {
    m_uniqueID = gHandleID++;
  }
}

FSoundEffectHandle::~FSoundEffectHandle()
{
  m_audioComponent.Reset();
}

bool FSoundEffectHandle::IsValid() const
{
  return m_uniqueID != INVALID_HANDLE_ID && 
         m_audioComponent.IsValid();
}