// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SoundEffectHandle.h"
#include "IAudioPlayer.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAudioPlayer : public UInterface
{
  GENERATED_BODY()
};

/**
 * 
 */
class MLIBRARY_API IAudioPlayer
{
  GENERATED_BODY()

  // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
  virtual FSoundEffectHandle PlaySE3D(const FString&, float volume = 1.0f, const FVector& location = FVector::ZeroVector) = 0;
  virtual FSoundEffectHandle PlaySE(const FString&, float volume = 1.0f) = 0;
  virtual bool StopSE(const FSoundEffectHandle&) = 0;
  virtual void PlayBGM(const FString&, float volume = 1.0f) = 0;
  virtual bool StopBGM() = 0;
};
