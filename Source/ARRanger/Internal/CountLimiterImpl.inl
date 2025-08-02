#pragma once

#ifdef _INTERNAL_CNT_LIMITER_INLINE_PROTECTOR_
  #error Can not include this in another file
#endif

#ifndef _INTERNAL_CNT_LIMITER_INLINE_PROTECTOR_
#define _INTERNAL_CNT_LIMITER_INLINE_PROTECTOR_

namespace ARRanger::Private
{
  template<typename UserType, uint8 MaxSize>
  FCountLimiter<UserType, MaxSize>::FCountLimiter()
  {
    ensureMsgf(UserType::NumInstance < MaxSize, TEXT("Create too many instances. Type:[%hs]"), typeid(UserType).name());
    ++UserType::NumInstance;
  }

  template<typename UserType, uint8 MaxSize>
  FCountLimiter<UserType, MaxSize>::~FCountLimiter()
  {
    --UserType::NumInstance;
  }

  template<typename UserType, uint8 MaxSize>
  uint8 FCountLimiter<UserType, MaxSize>::GetMaxSize()
  {
    return MaxSize;
  }

  template<typename UserType, uint8 MaxSize> 
  uint8 FCountLimiter<UserType, MaxSize>::GetCreatedObjectNum()
  {
    return UserType::NumInstance;
  }
}

#endif // _INTERNAL_CNT_LIMITER_INLINE_PROTECTOR_