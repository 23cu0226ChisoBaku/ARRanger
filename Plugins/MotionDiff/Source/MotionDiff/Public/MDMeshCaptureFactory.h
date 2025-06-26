// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDMeshCaptureFwd.h"
#include "MDMeshCaptureSupportedTypeTraits.h"


class MOTIONDIFF_API FMDMeshCaptureFactory
{
  public:
    template<typename UEMeshComponentType>
    static UMDMeshCapture* CreateCapture(UEMeshComponentType* MeshComponentPtr)
    {
      static_assert(IsSupportedMeshComponentCapture_V<UEMeshComponentType>, "Unsupported capture type");

      return CreateCaptureImpl(MeshComponentPtr);
    }

  private:
    static UMDMeshCapture* CreateCaptureImpl(UStaticMeshComponent* MeshComponentPtr);
    static UMDMeshCapture* CreateCaptureImpl(USkeletalMeshComponent* MeshComponentPtr);

  public:
    FMDMeshCaptureFactory() = delete;
    ~FMDMeshCaptureFactory() = delete;
};
