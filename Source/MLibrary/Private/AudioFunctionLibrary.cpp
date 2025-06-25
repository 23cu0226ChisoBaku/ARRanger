// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioFunctionLibrary.h"
#include "AudioKit.h"

using namespace MLibrary::UE::Audio;

void UAudioFunctionLibrary::PlaySE(const FString& seName, float volume)
{
  AudioKit::PlaySE(seName, volume);
}

void UAudioFunctionLibrary::PlaySE3D(const FString& seName, float volume, const FVector& location)
{
  AudioKit::PlaySE3D(seName, volume, location);
}

void UAudioFunctionLibrary::PlayBGM(const FString& bgmName, float volume)
{
  AudioKit::PlayBGM(bgmName, volume);
}

void UAudioFunctionLibrary::StopBGM()
{
  AudioKit::StopBGM();
}