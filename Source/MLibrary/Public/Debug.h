// Fill out your copyright notice in the Description page of Project Settings.
/*
Module MLibrary

Debug.h

Author : MAI ZHICONG

Description : Debug Wrapper class for Unreal Engine 5
Support verion: 5.3.2

Update History: 2024/12/03 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef M_LIB_DEBUG
#define M_LIB_DEBUG

#include "CoreMinimal.h"
#include "DebugFormat.h"

#include <string>

DECLARE_LOG_CATEGORY_EXTERN(MDebugLog, Log, All);

namespace UE::MLibrary
{
  class MLIBRARY_API Debug
  {
    private:
      enum class MLIB_DEBUG_LEVEL : uint8
      {
        Log,
        Warning,
        Error,
      };

    public:
      static void Log(const char*);
      static void Log(const wchar_t*);
      static void Log(const std::string&);
      static void Log(const std::wstring&);
      static void Log(const FString&);

      static void LogWarning(const char*);
      static void LogWarning(const wchar_t*);
      static void LogWarning(const std::string&);
      static void LogWarning(const std::wstring&);
      static void LogWarning(const FString&);

      static void LogError(const char*);
      static void LogError(const wchar_t*);
      static void LogError(const std::string&);
      static void LogError(const std::wstring&);
      static void LogError(const FString&);

    public:
      static float gLogTime;

    private:
      static void log_impl(MLIB_DEBUG_LEVEL, const FString&);

      static FString append_impl(const FString&, const char*);
      static FString append_impl(const FString&, const wchar_t*);
      static FString append_impl(const FString&, const std::string&);
      static FString append_impl(const FString&, const std::wstring&);
      static FString append_impl(const FString&, const FString&);
      
    private:
      static const DebugFormat LOG_FORMAT;
      static const DebugFormat LOG_WARNING_FORMAT;
      static const DebugFormat LOG_ERROR_FORMAT;
  };
}

using MDebug = typename UE::MLibrary::Debug;
#endif