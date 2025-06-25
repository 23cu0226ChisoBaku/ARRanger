// Fill out your copyright notice in the Description page of Project Settings.
/*

Dimension2Array.h

Author : MAI ZHICONG(バクチソウ)

Description : 2-Dimension array template class

Update History: 2024/11/27 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef M_DIMENSION_TWO_ARRAY
#define M_DIMENSION_TWO_ARRAY

#include "ArrayIterator.h"
#include "Dimension2ArrayHandle.h"

namespace UE::MLibrary
{
  namespace MDataStructure
  {
    ///
    /// @brief 二次元配列
    /// @param T データの型
    ///
    template<typename T>
    class TDimension2Array final
    {
      //---------------------------------------
      /*
                エイリアスやアサーション
      */
      //---------------------------------------
      public:
        using ElementType = T;
        using ElementTypePtr = ElementType*;
        using Iterator = ArrayIterator<TDimension2Array<ElementType>>;
        using ConstIterator = const Iterator;

      //---------------------------------------
      /*
                      ctorとdtor
      */
      //---------------------------------------
      public:
        TDimension2Array()
          : m_elemArr(nullptr)
          , m_row(0)
          , m_column(0)
        { }
        TDimension2Array(uint64 row, uint64 column)
          : m_elemArr(nullptr)
          , m_row(row)
          , m_column(column)
        {
          check( ((m_row * m_column) > 0) )

          const uint64 arrSize = m_row * m_column;
          m_elemArr = new ElementType[arrSize];
          memset(m_elemArr, 0, sizeof(ElementType) * arrSize);

        }
        TDimension2Array(uint64 row, uint64 column, const T& defaultValue)
          : m_elemArr(nullptr)
          , m_row(row)
          , m_column(column)
        {
          check( ((m_row * m_column) > 0) )

          const uint64 arrSize = m_row * m_column;
          m_elemArr = new ElementType[arrSize];
          for (uint64 i = 0; i < arrSize; ++i)
          {
            m_elemArr[i] = defaultValue;
          }

        }

        TDimension2Array(const ElementType* src, size_t dataCnt, uint64 row, uint64 column)
          : TDimension2Array(row, column)
        {
          check(src != nullptr)
          // サイズが０じゃない　かつ　サイズが （データのサイズ * 行の数 * 列の数）と等しい時に通過する
          check(((dataCnt != 0) && (dataCnt ==  row * column)))

          const size_t srcSize = sizeof(ElementType) * m_row * m_column;
          memcpy_s(m_elemArr, srcSize, src, srcSize);
        }

        TDimension2Array(const TDimension2Array& other)
          : m_elemArr(nullptr)
          , m_row(other.m_row)
          , m_column(other.m_column)
        {
          const uint64 arrSize = m_row * m_column;
          const size_t memSize = sizeof(ElementType) * arrSize;

          m_elemArr = new ElementType[arrSize];
          memcpy_s(m_elemArr, memSize, other.m_elemArr, memSize);
        }

        TDimension2Array& operator=(const TDimension2Array& other)
        {
          if (this != &other)
          {
            ElementTypePtr tempPtr = m_elemArr;

            m_elemArr = new ElementType[other.Length()];
            m_row = other.m_row;
            m_column = other.m_column;

            const uint64 arrSize = m_row * m_column;
            const size_t memSize = sizeof(ElementType) * arrSize;
            memcpy_s(m_elemArr, memSize, other.m_elemArr, memSize);

            delete[] tempPtr;
          }

          return *this;
        }

        TDimension2Array(TDimension2Array&& other) noexcept
          : m_elemArr(other.m_elemArr)
          , m_row(other.m_row)
          , m_column(other.m_column)
        {
          other.m_elemArr = nullptr;
          other.m_row = 0;
          other.m_column = 0;
        }
        TDimension2Array& operator=(TDimension2Array&& other) noexcept
        {
          if (this != &other)
          {
            delete[] m_elemArr;

            m_elemArr = other.m_elemArr;
            m_row = other.m_row;
            m_column = other.m_column;

            other.m_elemArr = nullptr;
            other.m_row = 0;
            other.m_column = 0;
          }

          return *this;
        }

        ~TDimension2Array()
        {
          dispose();
        }

    //---------------------------------------
    /*
            パブリック関数(クラス専用)
    */
    //---------------------------------------
      public:
        ///
        /// @brief 二次元配列の値を返す(読み込み専用)
        /// @param row 行のインデックス
        /// @param column 列のインデックス
        /// @return 配列[行のインデックス][列のインデックス]のコンスト参照
        ///
        const ElementType& At_ReadOnly(uint64 row, uint64 column) const &
        {
          return at_impl(row, column);
        }
        ///
        /// @brief 二次元配列の値を返す(読み込み専用)
        /// @param row 行のインデックス
        /// @param column 列のインデックス
        /// @return 配列[行のインデックス][列のインデックス]のコンスト参照
        ///
        const ElementType& At(uint64 row, uint64 column) const &
        {
          return at_impl(row, column);
        }
        /// @brief 二次元配列の値を返す(書き込み可能)
        /// @param row 行のインデックス
        /// @param column 列のインデックス
        /// @return 配列[行のインデックス][列のインデックス]の参照
        ElementType& At(uint64 row, uint64 column) &
        {
          return at_impl(row, column);
        }
        /// @brief 二次元配列の値を返す(書き込み可能)
        /// @param row 行のインデックス
        /// @param column 列のインデックス
        /// @return 配列[行のインデックス][列のインデックス]の参照
        ElementType At(uint64 row, uint64 column) &&
        {
          return at_impl_copy(row, column);
        }
        /// @brief 配列の行数を返す(読み込み専用)
        /// @return 配列の行数
        uint64 Row() const
        {
          return m_row;
        }
        /// @brief 配列の列数を返す(読み込み専用)
        /// @return 配列の列数				
        uint64 Column() const
        {
          return m_column;
        }
        /// @brief 配列の長さを返す(読み込み専用)
        /// @return 配列の長さ
        uint64 Length() const
        {
          return m_row * m_column;
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
          return Iterator(m_elemArr + m_row * m_column);
        }
        ConstIterator end() const
        {
          return ConstIterator(m_elemArr + m_row * m_column);
        }
        TDimension2ArrayHandle<ElementType> GetData() const
        {
          return TDimension2ArrayHandle<ElementType>(m_elemArr, m_row, m_column);
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

      // Private
      #pragma region Private
      private:
        void dispose() noexcept
        {
          if (m_elemArr != nullptr)
          {
            delete[] m_elemArr;
          }

          memset(this, 0, sizeof(*this));
        }

        ElementType& at_impl(uint64 row, uint64 column)
        {
          check(m_elemArr != nullptr)
          check(row < m_row && column < m_column)

          return m_elemArr[row * m_column + column];

        }

        const ElementType& at_impl(uint64 row, uint64 column) const
        {
          check(m_elemArr != nullptr)
          check(row < m_row && column < m_column)

          return m_elemArr[row * m_column + column];

        }

        ElementType at_impl_copy(uint64 row, uint64 column) const
        {
          check(m_elemArr != nullptr)
          check(row < m_row && column < m_column)

          return m_elemArr[row * m_column + column];

        }

      //---------------------------------------
      /*
              スタティック変数(public)
      */
      //---------------------------------------
      public:
        static const TDimension2Array<ElementType> EmptyArray;

      private:
        ElementType* m_elemArr;
        uint64 m_row;
        uint64 m_column;

      #pragma endregion
      // Private
    };

    template<typename T>
    const TDimension2Array<T> TDimension2Array<T>::EmptyArray = TDimension2Array<T>{};
  }
}

#endif