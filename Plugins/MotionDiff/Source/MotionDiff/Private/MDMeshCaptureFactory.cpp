// Fill out your copyright notice in the Description page of Project Settings.

#include "MDMeshCaptureFactory.h"

#include "MeshCapture/MDStaticMeshCapture.h"
#include "MeshCapture/MDSkeletalMeshCapture.h"

UMDMeshCapture* FMDMeshCaptureFactory::CreateCaptureImpl(UObject* Owner, const UStaticMeshComponent* MeshComponentPtr)
{
  return NewObject<UMDStaticMeshCapture>(Owner);
}

UMDMeshCapture* FMDMeshCaptureFactory::CreateCaptureImpl(UObject* Owner, const USkeletalMeshComponent* MeshComponentPtr)
{
  return NewObject<UMDSkeletalMeshCapture>(Owner);
}