// Fill out your copyright notice in the Description page of Project Settings.

#include "MDMeshCaptureFactory.h"

#include "MeshCapture/MDStaticMeshCapture.h"

UMDMeshCapture* FMDMeshCaptureFactory::CreateCaptureImpl(UObject* Owner, const UStaticMeshComponent* MeshComponentPtr)
{
  return NewObject<UMDStaticMeshCapture>(Owner);
}

UMDMeshCapture* FMDMeshCaptureFactory::CreateCaptureImpl(UObject* Owner, const USkeletalMeshComponent* MeshComponentPtr)
{
  // FIXME need implementation
  return nullptr;
}