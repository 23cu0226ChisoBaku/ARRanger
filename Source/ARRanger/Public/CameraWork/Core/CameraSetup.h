/**
 * MCW camera setup
 * 
 * // NOTE : Current environment: Unreal engine 5.6
 */

#pragma once

#ifndef _MCW_CORE_CAMERA_SETUP_
#define _MCW_CORE_CAMERA_SETUP_

#include "Core/CoreDefines.h"

#define 

namespace AR
{
namespace CameraWork
{
  #if IS_UE_ENVIRONMENT
    namespace Private
    {
      using MCW_Vector = FVector;
      using MCW_Rotator = FRotator;
      
      template<typename T>
      constexpr T Default()
      {
        return static_cast<T>(0);
      }

      template<FVector>
      constexpr FVector Default()
      {
        return FVector{0.0, UE::Math::TVectorConstInit{}};
      }

      template<FRotator>
      constexpr FRotator Default()
      {
        return FRotator{0.0};
      }
    }
  #else
    namespace Private
    {

    }
  #endif

  struct FCameraSetup final
  {
    public:
      FCameraSetup();
      ~FCameraSetup();
      void Reset();

    public:


    private:
      Private::MCW_Vector Location = Private::Default<Private::MCW_Vector>();

      Private::MCW_Rotator Rotation = Private::Default<Private::MCW_Rotator>();
  };  
} 
}

#endif // _MCW_CORE_CAMERA_SETUP_