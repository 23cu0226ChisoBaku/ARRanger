// Fill out your copyright notice in the Description page of Project Settings.
/*

Dimension1Array.h

Author : MAI ZHICONG(バクチソウ)

Description : 1-Dimension array template class

Update History: 2024/11/26 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef M_DIMENSION_ONE_ARRAY
#define M_DIMENSION_ONE_ARRAY

#include "ArrayIterator.h"

#define ThrowWhenTrue(expression) \
if (!!(expression))								\
{																	\
  throw;													\
}

#define ThrowWhenFalse(expression) ThrowWhenTrue(!expression)

namespace UE::MLibrary
{
  namespace MDataStructure
  {
    ///
    /// @brief 一次元配列
    /// @param T データの型
    ///
    template<typename T>
    class TDimension1Array final
    {

    //---------------------------------------
    /*
              エイリアスやアサーション
    */
    //---------------------------------------
    public:
      using ElementType = T;
      using Iterator = ArrayIterator<TDimension1Array<ElementType>>;
      using ConstIterator = const Iterator;
      // static_assert(std::is_integral_v<ElementType> || std::is_floating_point_v<ElementType>, "Can't use type that is not integer or floating point");

    //---------------------------------------
    /*
                    ctorとdtor
    */
    //---------------------------------------
      public:
        TDimension1Array() noexcept
          : m_elemArr(nullptr)
          , m_length(0)
        { }
        explicit TDimension1Array(uint64 length)
          : m_elemArr(nullptr)
          , m_length(length)
        {
          m_elemArr = new ElementType[m_length];
          memset(m_elemArr, 0, sizeof(ElementType) * m_length);
        }

        TDimension1Array(const TDimension1Array& other)
          : m_elemArr(nullptr)
          , m_length(other.m_length)
        {
          m_elemArr = new ElementType[m_length];

          const size_t cpySize = sizeof(ElementType) * m_length;
          memcpy_s(m_elemArr, cpySize, other.m_elemArr, cpySize);	
        }
        TDimension1Array& operator=(const TDimension1Array& other)
        {
          if (this != &other)
          {
            if (m_elemArr != nullptr)
            {
              delete[] m_elemArr;
            }
            m_elemArr = new ElementType[other.m_length];
            m_length = other.m_length;
            
            const size_t cpySize = sizeof(ElementType) * m_length;
            memcpy_s(m_elemArr, cpySize, other.m_elemArr, cpySize);	
          }

          return *this;
        }

        TDimension1Array(TDimension1Array&& other) noexcept
          : m_elemArr(other.m_elemArr)
          , m_length(other.m_length)
        {					
          other.m_elemArr = nullptr;
          other.m_length = 0;
        }
        TDimension1Array& operator=(TDimension1Array&& other) noexcept
        {
          if (this != &other)
          {
            if (m_elemArr != nullptr)
            {
              delete[] m_elemArr;
            }

            m_elemArr = other.m_elemArr;
            m_length = other.m_length;

            other.m_elemArr = nullptr;
            other.m_length = 0;
          }

          return *this;
        }

        ~TDimension1Array()
        {
          dispose();
        }

    //---------------------------------------
    /*
            パブリック関数(クラス専用)
    */
    //---------------------------------------
      public:
        /// @brief 配列の値を返す(読み込み専用)
        /// @param idx インデックス
        /// @return 配列[インデックス]のコンスト参照
        const ElementType& At_ReadOnly(uint64 idx) const &
        {
          return at_impl(idx);
        }
        /// @brief 配列の値を返す(読み込み専用)
        /// @param idx インデックス
        /// @return 配列[インデックス]のコンスト参照
        const ElementType& At(uint64 idx) const &
        {
          return at_impl(idx);
        }
        /// @brief 配列の値を返す(書き換え可能)
        /// @param idx インデックス
        /// @return 配列[インデックス]の参照
        ElementType& At(uint64 idx) &
        {
          return at_impl(idx);
        }
        /// @brief 配列の値を返す(書き換え可能)
        /// @param idx インデックス
        /// @return 配列[インデックス]の参照
        ElementType At(uint64 idx) &&
        {
          return at_impl_copy(idx);
        }
        /// @brief 配列の長さを返す(読み込み専用)
        /// @return 配列の長さ
        uint64 Length() const
        {
          return m_length;
        }

    // イテレータ
        Iterator begin()
        {
          return Iterator(m_elemArr);
        }
        ConstIterator begin() const
        {
          return ConstIterator(m_elemArr);
        }
        Iterator end()
        {
          return Iterator(m_elemArr + m_length);
        }
        ConstIterator end() const
        {
          return ConstIterator(m_elemArr + m_length);
        }
    //---------------------------------------
    /*
            パブリック関数(インターフェース)
    */
    //---------------------------------------
      public:

    //---------------------------------------
    /*
            オペレーターオーバーロード
    */
    //---------------------------------------
      public:
        /// @brief 配列の値を返す(読み込み専用)
        /// @param idx インデックス
        /// @return 配列[インデックス]のコンスト参照
        const ElementType& operator[](uint64 idx) const&
        {
            return at_impl(idx);
        }
        /// @brief 配列の値を返す(書き換え可能)
        /// @param idx インデックス
        /// @return 配列[インデックス]の参照
        ElementType& operator[](uint64 idx) &
        {
            return at_impl(idx);
        }
        /// @brief 配列の値を返す(書き換え可能)
        /// @param idx インデックス
        /// @return 配列[インデックス]の参照
        ElementType operator[](uint64 idx) &&
        {
            return at_impl_copy(idx);
        }
    // Private
    #pragma region Private
      private:
        void dispose() noexcept
        {
          if (m_elemArr != nullptr)
          {
            delete[] m_elemArr;

          }

          m_elemArr = nullptr;
          m_length = 0;
        }
        ElementType& at_impl(uint64 idx)
        {
          check(idx < m_length)

          return m_elemArr[idx];
        }

        ElementType at_impl_copy(uint64 idx)
        {
          check(idx < m_length)

          return m_elemArr[idx];
        }

      private:
        ElementType* m_elemArr;
        uint64 m_length;

    #pragma endregion
    // Private
    };
  }
}
#endif