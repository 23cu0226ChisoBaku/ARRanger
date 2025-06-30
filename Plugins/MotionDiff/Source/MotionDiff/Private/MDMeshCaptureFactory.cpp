// Fill out your copyright notice in the Description page of Project Settings.

#include "MDMeshCaptureFactory.h"

#include "MeshCapture/MDStaticMeshCapture.h"

UMDMeshCapture* FMDMeshCaptureFactory::CreateCaptureImpl(UStaticMeshComponent* MeshComponentPtr)
{
  return NewObject<UMDStaticMeshCapture>();
}

UMDMeshCapture* FMDMeshCaptureFactory::CreateCaptureImpl(USkeletalMeshComponent* MeshComponentPtr)
{
  // FIXME need implementation
  return nullptr;
}