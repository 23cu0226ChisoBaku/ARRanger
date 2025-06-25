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

FSoundEffectHandle::FSoundEffectHandle()
  : UniqueIdentifier{}
  , AudioComponent(nullptr)
{}
FSoundEffectHandle::FSoundEffectHandle(const FString& identifierStr, UAudioComponent* audioComp)
  : UniqueIdentifier(identifierStr)
  , AudioComponent(audioComp)
{}
FSoundEffectHandle::~FSoundEffectHandle()
{
  UniqueIdentifier.Invalidate();
  AudioComponent.Reset();
}

bool IsEqual(const FSoundEffectHandle& lhs, const FSoundEffectHandle& rhs)
{
  return (lhs.UniqueIdentifier == rhs.UniqueIdentifier) && (lhs.AudioComponent == rhs.AudioComponent);
}