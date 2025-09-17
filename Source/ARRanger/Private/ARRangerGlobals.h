// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define UE_API ARRANGER_API

namespace ARRanger
{

namespace Global
{
  extern UE_API void RegisterDebugKey();
  extern UE_API void UnregisterDebugKey();
} // namespace ARRanger::Global

} // namespace ARRanger

#undef UE_API