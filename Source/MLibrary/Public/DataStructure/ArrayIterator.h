// Fill out your copyright notice in the Description page of Project Settings.
/*

ArrayIterator.h

Author : MAI ZHICONG(バクチソウ)

Description : Iterator of TDimension[X]Array (X >= 1)

Update History: 2024/12/01 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

///
/// @brief 配列イテレータ
/// @param Array 配列の型 (TDimension[X]Array) (X >= 1)
///
template <typename Array>
class ArrayIterator
{
  using ElementType = typename Array::ElementType;
  using PointerType = ElementType*;
  using ReferenceType = ElementType&;
  using ConstReferenceType = const ReferenceType;
public:
  ArrayIterator(PointerType ptr)
    : m_ptr(ptr)
  { }

  PointerType Get() const
  {
    return m_ptr;
  }

  ArrayIterator& operator++()
  {
    m_ptr++;
    return *this;
  }

  ArrayIterator operator++(int)
  {
    ArrayIterator itr = *this;
    ++(*this);
    return itr;
  }

    ArrayIterator& operator--()
  {
    m_ptr--;
    return *this;
  }

  ArrayIterator operator--(int)
  {
    ArrayIterator itr = *this;
    --(*this);
    return itr;
  }

  ConstReferenceType operator[](uint64 idx) const&
  {
    return *(m_ptr[idx]);
  }

  ReferenceType operator[](uint64 idx) &
  {
    return *(m_ptr[idx]);
  }

  ReferenceType operator[](uint64 idx) &&
  {
    return *(m_ptr[idx]);
  }

  PointerType operator->()
  {
    return m_ptr;
  }

  ReferenceType operator*()
  {
    return *m_ptr;
  }

  bool operator==(const ArrayIterator& other) const
  {
    return m_ptr == other.m_ptr;
  }

  bool operator!=(const ArrayIterator& other) const
  {
    return !(*this == other);
  }


private:
  PointerType m_ptr;
};
