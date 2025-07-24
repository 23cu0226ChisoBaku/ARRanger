#pragma once

#ifndef _MENGINE_CORE_TEMPLATES_CAST_
#define _MENGINE_CORE_TEMPLATES_CAST_

#include <type_traits>

namespace MEngine
{
namespace Core
{
  template<typename FromType, typename ToType>
  __forceinline ToType* CastFast(FromType* SrcPtr)
  {
    static_assert(sizeof(FromType) > 0 && sizeof(ToType) > 0, "Cast between incomplete type is illegal");

    if (SrcPtr != nullptr)
    {
      // Static cast if ToType is a base type of FromType or same type
      if constexpr (std::is_base_of_v<ToType, FromType>)
      {
        return static_cast<ToType*>(SrcPtr);
      }
      else
      {
        // Dynamic cast if ToType is a devired class of FromType
        if constexpr (std::is_base_of_v<FromType, ToType>)
        {
          return dynamic_cast<ToType*>(SrcPtr);
        }
      }
    }
    
    return nullptr;
  }
}
}

using MEngine::Core::CastFast;

#endif // _MENGINE_CORE_TEMPLATES_CAST_