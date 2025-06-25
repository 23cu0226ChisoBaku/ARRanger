// Fill out your copyright notice in the Description page of Project Settings.
/*

SoundEffectHandle.h

Author : MAI ZHICONG

Description : SEハンドル

Update History: 2025/02/07 作成

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef MLIBRARY_SE_HANDLE
#define MLIBRARY_SE_HANDLE

#include "CoreMinimal.h"

class FSoundEffectHandle
{
public:
  MLIBRARY_API FSoundEffectHandle();
  MLIBRARY_API FSoundEffectHandle(const FString& identifierStr, UAudioComponent* audioComp);
  MLIBRARY_API ~FSoundEffectHandle();

  bool operator==(const FSoundEffectHandle& rhs)
  {
    return IsEqual(*this, rhs);
  }
  UAudioComponent* operator->() const
  {
    return AudioComponent.Get();
  }
  bool IsValid() const
  {
    return UniqueIdentifier.IsValid() && AudioComponent.IsValid();
  }

  MLIBRARY_API friend bool IsEqual(const FSoundEffectHandle&, const FSoundEffectHandle&);
private:
  /// @brief	ハンドルユニークID 
  FGuid UniqueIdentifier;
  /// @brief	SE再生に使うAudioComponentへのポインタ
  TWeakObjectPtr<UAudioComponent> AudioComponent;
};

#endif
