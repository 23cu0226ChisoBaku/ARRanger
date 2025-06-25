// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef M_DIMENSION_TWO_ARRAY_HANDLE
#define M_DIMENSION_TWO_ARRAY_HANDLE

namespace UE::MLibrary
{
  namespace MDataStructure
  {
    template<typename ElementType>
    struct TDimension2ArrayHandle
    {
      const ElementType* Ptr;
      uint64 Row;
      uint64 Column;

      constexpr TDimension2ArrayHandle()
        : Ptr(nullptr)
        , Row(0u)
        , Column(0u)
      { }

      TDimension2ArrayHandle(ElementType* ptr, uint64 row, uint64 column)
        : Ptr(ptr)
        , Row(row)
        , Column(column)
      { }
    };
  }
}

#endif
