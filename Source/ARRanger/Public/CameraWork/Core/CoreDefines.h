// Code name MCW

#pragma once

#ifndef _MCW_CAMERAWORK_CORE_DEFINES_
#define _MCW_CAMERAWORK_CORE_DEFINES_

// TODO: Modify this later
#define USE_AS_DLL 0
#define MCW_DLL_EXPORT 0

#if USE_AS_DLL
  #define MCW_DLLIMPORT __declspec(dllimport)
  #define MCW_DLLEXPORT __declspec(dllexport)
#else
  #define MCW_DLLIMPORT
  #define MCW_DLLEXPORT
#endif // USE_AS_DLL

#if USE_AS_DLL
  #if MCW_DLL_EXPORT
    #define MCW_API MCW_DLLEXPORT
  #else
    #define MCW_API MCW_DLLIMPORT
  #endif // MCW_DLL_EXPORT  
#endif // USE_AS_DLL

// Fallback to empty macro if this is not dll
#ifndef MCW_API
  #define MCW_API
#endif // MCW_API

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEBUGGING) || defined(UE_BUILD_DEVELOPMENT) || defined(UE_BUILD_SHIPPING) || defined(UE_BUILD_TEST)
  #define IS_UE_ENVIRONMENT 1
#else
  #define IS_UE_ENVIRONMENT 0
#endif // defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEBUGGING) || defined(UE_BUILD_DEVELOPMENT) || defined(UE_BUILD_SHIPPING) || defined(UE_BUILD_TEST)

// Fallback if not defined
#ifndef IS_UE_ENVIRONMENT
  #define IS_UE_ENVIRONMENT 0
#endif

// Assertion
#if IS_UE_ENVIRONMENT
  #define MCW_CHECK(Expression) check(!!(Expression))
#elif defined(_MENGINE_VER) // MEngine assertion
  #define MCW_CHECK(Expression) me_assert(!!(Expression))
#endif

// Fallback to c++ standard assertion
#ifndef MCW_CHECK
  #include <cassert>
  #define MCW_CHECK(Expression) assert(!!(Expression))
#endif

// FIXME: Temporary use
#include "Core/Cast.h"

#endif // _MCW_CAMERAWORK_CORE_DEFINES_