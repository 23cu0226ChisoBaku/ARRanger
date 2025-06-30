// Fill out your copyright notice in the Description page of Project Settings.

#include "MeshCapture/MDMeshCapture.h"
#include "MDMeshSnapshot.h"
#include "MeshCapture/MDMeshCaptureProxy.h"

#include "MotionDiff/MotionDiffLogChannels.h"


UMDMeshCapture::UMDMeshCapture(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , m_meshCaptureProxy{nullptr}
{
}

void UMDMeshCapture::BeginDestroy()
{
  Super::BeginDestroy();
  
  if (m_meshCaptureProxy != nullptr)
  {
    DestroyMeshCaptureProxy(m_meshCaptureProxy);
    m_meshCaptureProxy = nullptr;
  }
}

void UMDMeshCapture::SaveMeshSnapshot(FName SnapshotName)
{
  FMDMeshCaptureProxy& proxy = GetMeshCaptureProxy<FMDMeshCaptureProxy>();
  proxy.SaveMeshSnapshot(this, SnapshotName);
}

FMDMeshCaptureProxy* UMDMeshCapture::CreateMeshCaptureProxy()
{
  return new FMDMeshCaptureProxy();
}

void UMDMeshCapture::DestroyMeshCaptureProxy(FMDMeshCaptureProxy* MeshCaptureProxy)
{
  delete MeshCaptureProxy;
}
