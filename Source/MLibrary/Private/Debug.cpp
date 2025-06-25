// Fill out your copyright notice in the Description page of Project Settings.
/*
Module MLibrary

Debug.cpp

Author : MAI ZHICONG

Description : Debug Wrapper class for Unreal Engine 5
Support verion: 5.3.2

Update History: 2024/12/03 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include "Debug.h"

DEFINE_LOG_CATEGORY(MDebugLog);

namespace UE::MLibrary
{
  float Debug::gLogTime = 15.0f;
  // Log 
  #pragma region Log

    const DebugFormat Debug::LOG_FORMAT = { TEXT("[Log]:"), FColor::White };

    void Debug::Log(const char* msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Log, append_impl(LOG_FORMAT.LogHeaderString, msg));
    }
    void Debug::Log(const wchar_t* msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Log, append_impl(LOG_FORMAT.LogHeaderString, msg));
    }
    void Debug::Log(const std::string& msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Log, append_impl(LOG_FORMAT.LogHeaderString, msg));
    }
    void Debug::Log(const std::wstring& msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Log, append_impl(LOG_FORMAT.LogHeaderString, msg));
    }
    void Debug::Log(const FString& msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Log, append_impl(LOG_FORMAT.LogHeaderString, msg));
    }

    void Debug::log_impl(MLIB_DEBUG_LEVEL debugLevel, const FString& log)
    {
      #if WITH_EDITOR
        if (GEngine == nullptr)
        {
          return;
        }

        switch (debugLevel)
        {
          case MLIB_DEBUG_LEVEL::Log:
          {
            GEngine->AddOnScreenDebugMessage(-1, gLogTime, LOG_FORMAT.LogColor, log);
            UE_LOG(MDebugLog, Log, TEXT("%s"), *log);
          }
          break;
          case MLIB_DEBUG_LEVEL::Warning:
          {
            GEngine->AddOnScreenDebugMessage(-1, gLogTime, LOG_WARNING_FORMAT.LogColor, log);
            UE_LOG(MDebugLog, Warning, TEXT("%s"), *log); 
          }
          break;
          case MLIB_DEBUG_LEVEL::Error:
          {
            GEngine->AddOnScreenDebugMessage(-1, gLogTime, LOG_ERROR_FORMAT.LogColor, log); 
            UE_LOG(MDebugLog, Error, TEXT("%s"), *log);
          }
          break;
        }
      #endif
    }

  #pragma endregion
  // endregion of log

  // Log Warning
  #pragma region Log Warning

    const DebugFormat Debug::LOG_WARNING_FORMAT = { TEXT("[Warning]:"), FColor::Yellow };

    void Debug::LogWarning(const char* msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Warning, append_impl(LOG_WARNING_FORMAT.LogHeaderString, msg));
    }
    void Debug::LogWarning(const wchar_t* msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Warning, append_impl(LOG_WARNING_FORMAT.LogHeaderString, msg));
    }
    void Debug::LogWarning(const std::string& msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Warning, append_impl(LOG_WARNING_FORMAT.LogHeaderString, msg));
    }
    void Debug::LogWarning(const std::wstring& msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Warning, append_impl(LOG_WARNING_FORMAT.LogHeaderString, msg));
    }
    void Debug::LogWarning(const FString& msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Warning, append_impl(LOG_WARNING_FORMAT.LogHeaderString, msg));
    }

  #pragma endregion
  // endregion of Log Warning

  // Log Error
  #pragma region Log Error

    const DebugFormat Debug::LOG_ERROR_FORMAT = { TEXT("[Error]:"), FColor::Red };

    void Debug::LogError(const char* msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Error, append_impl(LOG_ERROR_FORMAT.LogHeaderString, msg));
    }
    void Debug::LogError(const wchar_t* msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Error, append_impl(LOG_ERROR_FORMAT.LogHeaderString, msg));
    }
    void Debug::LogError(const std::string& msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Error, append_impl(LOG_ERROR_FORMAT.LogHeaderString, msg));
    }
    void Debug::LogError(const std::wstring& msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Error, append_impl(LOG_ERROR_FORMAT.LogHeaderString, msg));
    }
    void Debug::LogError(const FString& msg)
    {
      log_impl(MLIB_DEBUG_LEVEL::Error, append_impl(LOG_ERROR_FORMAT.LogHeaderString, msg));
    }

  #pragma endregion
  // endregion of Log Error

  // Append Implementation
  #pragma region Append Impl
    FString Debug::append_impl(const FString& logFormat, const char* msg)
    {
      return logFormat + msg;
    }
    FString Debug::append_impl(const FString& logFormat, const wchar_t* msg)
    {
      return logFormat + msg;
    }
    FString Debug::append_impl(const FString& logFormat, const std::string& msg)
    {
      return logFormat + msg.c_str();
    }
    FString Debug::append_impl(const FString& logFormat, const std::wstring& msg)
    {
      return logFormat + msg.c_str();
    }
    FString Debug::append_impl(const FString& logFormat, const FString& msg)
    {
      return logFormat + msg;
    }
  #pragma endregion
  // endregion of Append Implementation
}


