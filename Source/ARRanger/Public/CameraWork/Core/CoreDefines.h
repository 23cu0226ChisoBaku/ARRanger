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

#endif // _MCW_CAMERAWORK_CORE_DEFINES_