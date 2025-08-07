#pragma once

#ifndef _AR_INTERNAL_COUNT_LIMITER_
#define _AR_INTERNAL_COUNT_LIMITER_

namespace ARRanger::Private
{
  /**
   * @brief インスタンス生成の数を制限するクラス
   * 
   * @tparam UserType 使用するインスタンスの型
   * @tparam MaxSize  制限する最大数
   */
  template<typename UserType, uint8 MaxSize>
  class FCountLimiter
  {
    public:
      FCountLimiter();
      ~FCountLimiter();

      /**
       * @brief 最大数を取得
       * 
       * @return MaxSizeを返す
       */
      static uint8 GetMaxSize();

      /**
       * @brief 生成されたインスタンスの数
       * 
       * @return 使用クラスに定義した数カウンターを返す
       */
      static uint8 GetCreatedObjectNum();
  };
}

/**
 * CountLimiterプロパティ宣言 
 * クラスの内部で使用
 */
#define DECLARE_COUNT_LIMITER_PROPERTY(UserType, MaxSize) \
  private: \
    friend class ARRanger::Private::FCountLimiter<UserType, MaxSize>; \
    static uint8 NumInstance; \
  public: \
    using ARRanger::Private::FCountLimiter<UserType, MaxSize>::GetMaxSize; \
    using ARRanger::Private::FCountLimiter<UserType, MaxSize>::GetCreatedObjectNum; \
  private: 

/**
 * CountLimiterプロパティ定義
 * .cppで定義
 */
#define DEFINE_COUNT_LIMITER_PROPERTY(UserType) \
  uint8 UserType::NumInstance = 0;

// Include FCountLimiter inline file
#include "Internal/CountLimiterImpl.inl"

#endif // _AR_INTERNAL_COUNT_LIMITER_