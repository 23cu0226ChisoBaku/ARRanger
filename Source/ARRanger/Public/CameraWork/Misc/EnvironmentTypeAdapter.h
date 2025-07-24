#pragma once

#ifndef _MCW_MISC_ENVIRONMENT_TYPE_ADAPTER_
#define _MCW_MISC_ENVIRONMENT_TYPE_ADAPTER_

#include "Core/CoreDefines.h" 

namespace AR
{
namespace CameraWork
{
  #if IS_UE_ENVIRONMENT
    class FCameraModeRootNode;

    namespace Private
    {
      using OwnerType = UObject;
      using StrongOwnerPtr = TObjectPtr<OwnerType>;
      using WeakOwnerPtr = TWeakObjectPtr<OwnerType>;
      
      template<typename T>
      using SharedPtr = TSharedPtr<T>;
      
      template<typename T>
      using WeakPtr = TWeakPtr<T>;
      
      // Root node factory delegate
      using RootNodeFactoryMethod = TDelegate<FCameraModeRootNode*()>;

      #pragma region Camera mode view adapter
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
      #pragma endregion Camera mode view adapter
    }
  #elif defined(_MENGINE_VER)
    namespace Private
    {
      template<typename T>
      using SharedPtr = std::shared_ptr<T>;

      template<typename T>
      using WeakPtr = std::weak_ptr<T>;
    }
  #else
    #if !defined(_MEMORY_)
      #error Include <memory> for shared_ptr and weak_ptr
    #endif
    namespace Private
    {
      template<typename T>
      using SharedPtr = std::shared_ptr<T>;

      template<typename T>
      using WeakPtr = std::weak_ptr<T>;
    }
  #endif
}
}

#endif // _MCW_MISC_ENVIRONMENT_TYPE_ADAPTER_