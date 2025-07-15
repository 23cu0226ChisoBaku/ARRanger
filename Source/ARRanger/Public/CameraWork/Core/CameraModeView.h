/**
 * MCW camera mode view
 */

#pragma once

#ifndef _MCW_CORE_CAMERA_MODE_VIEW_
#define _MCW_CORE_CAMERA_MODE_VIEW_

#include "Core/CoreDefines.h"

namespace AR
{
namespace CameraWork
{
  // NOTE : Current environment: Unreal engine 5.6
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

  #define MCW_CAMERA_TRANSFORM_PROPERTIES_DEFINITION() \
          MCW_CAMERA_PROPERTY_DEFINITION(Private::MCW_Vector, Location) \
          MCW_CAMERA_PROPERTY_DEFINITION(Private::MCW_Rotator, Rotation) 

  #define MCW_CAMERA_ALL_PROPERTIES_DEFINITION() \
          MCW_CAMERA_TRANSFORM_PROPERTIES_DEFINITION()


  struct FCameraModeView final
  {
    // NOTE: std::remove_cvref only for c++20
    #define MCW_CAMERA_PROPERTY_DEFINITION(PropertyType, PropertyName) \
      PropertyType Get##PropertyName() const \
      { \
        return PropertyName; \
      } \
      void Set##PropertyName(std::remove_cvref<PropertyType>::type Value) \
      { \
        if ( PropertyName != Value ) \
        { \
          PropertyName = Value; \
        } \
      }
    
    /**
     * Properties getter/setter definition
     */
    public:
      MCW_CAMERA_ALL_PROPERTIES_DEFINITION()

    #undef MCW_CAMERA_PROPERTY_DEFINITION

    public:
      FCameraModeView();
      ~FCameraModeView() = default;
      void Reset();

    public:
      

    private:
      Private::MCW_Vector Location = Private::Default<decltype(Location)>();

      Private::MCW_Rotator Rotation = Private::Default<decltype(Rotation)>();
  };  
} 
}

#endif // _MCW_CORE_CAMERA_MODE_VIEW_