// NOTE : Environment: Unreal engine 5.6

#pragma once

#ifndef _MCW_CAMERAWORK_CORE_CAMERA_MODE_VIEW_
#define _MCW_CAMERAWORK_CORE_CAMERA_MODE_VIEW_

#include "Core/CoreDefines.h"

#include "Misc/EnvironmentTypeAdapter.h"

namespace AR
{
namespace CameraWork
{

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

    private:
      
      /**
       * Current camera transform location
       */
      Private::MCW_Vector Location = Private::Default<decltype(Location)>();

      /**
       * Current camera transform rotation
       */
      Private::MCW_Rotator Rotation = Private::Default<decltype(Rotation)>();
  };  
} 
}

#endif // _MCW_CAMERAWORK_CORE_CAMERA_MODE_VIEW_