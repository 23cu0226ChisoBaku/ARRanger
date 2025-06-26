// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <type_traits>

template<typename SupportedMeshComponentType>
constexpr bool IsSupportedCaptureImpl()
{
  if constexpr (
        std::is_same_v<SupportedMeshComponentType, UStaticMeshComponent>
    ||  std::is_same_v<SupportedMeshComponentType, USkeletalMeshComponent>
  )
  {
    return true;
  }
  else
  {
    return false;
  }
}

template<typename SupportedMeshComponentType>
struct IsSupportedMeshComponentCapture
{
  static_assert(std::is_base_of_v<UMeshComponent, SupportedMeshComponentType>, "Unsupported UMeshComponent type");

  constexpr static bool Value = IsSupportedCaptureImpl<SupportedMeshComponentType>();
};

template<typename SupportedMeshComponentType>
constexpr bool IsSupportedMeshComponentCapture_V = IsSupportedMeshComponentCapture<SupportedMeshComponentType>::Value;