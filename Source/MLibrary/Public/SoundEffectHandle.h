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

#ifndef _MLIBRARY_SE_HANDLE_
#define _MLIBRARY_SE_HANDLE_

#include "CoreMinimal.h"

class FSoundEffectHandle
{
  public:
    MLIBRARY_API static const FSoundEffectHandle InvalidHandle;
    MLIBRARY_API FSoundEffectHandle();
    MLIBRARY_API FSoundEffectHandle(UAudioComponent* audioComp);
    MLIBRARY_API ~FSoundEffectHandle();

    MLIBRARY_API bool IsValid() const;
    friend bool operator==(const FSoundEffectHandle& Lhs, const FSoundEffectHandle& Rhs);
    UAudioComponent* operator->() const { return m_audioComponent.Get(); }

  private:
    /// @brief	ハンドルユニークID 
    int64 m_uniqueID;
    /// @brief	SE再生に使うAudioComponentへのポインタ
    TWeakObjectPtr<UAudioComponent> m_audioComponent;
};

inline bool operator==(const FSoundEffectHandle& Lhs, const FSoundEffectHandle& Rhs)
{
  return (Lhs.m_uniqueID == Rhs.m_uniqueID) && (Lhs.m_audioComponent == Rhs.m_audioComponent);
}

#endif // _MLIBRARY_SE_HANDLE_
