// Fill out your copyright notice in the Description page of Project Settings.

#include "MeshCapture/MDMeshCapture.h"
#include "MDMeshSnapshot.h"
#include "MeshCapture/MDMeshCaptureProxy.h"

#include "MotionDiff/MotionDiffLogChannels.h"


UMDMeshCapture::UMDMeshCapture(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , m_meshCaptureProxy{nullptr}
  , m_materials{}
{
}

void UMDMeshCapture::BeginDestroy()
{
  if (m_meshCaptureProxy != nullptr)
  {
    DestroyMeshCaptureProxy(m_meshCaptureProxy);
    m_meshCaptureProxy = nullptr;
  }

  m_materials.Empty();
  
  Super::BeginDestroy();
}

void UMDMeshCapture::SaveMeshSnapshot(FName SnapshotName)
{
  FMDMeshCaptureProxy& proxy = GetMeshCaptureProxy<FMDMeshCaptureProxy>();
  proxy.SaveMeshSnapshot(this, SnapshotName);
}

void UMDMeshCapture::SetMaterials(const TArray<FMDMeshCaptureMaterial>& Materials)
{
  m_materials = Materials;
}

const TArray<FMDMeshCaptureMaterial>& UMDMeshCapture::GetMaterials() const
{
  return m_materials;
}

FMDMeshCaptureProxy* UMDMeshCapture::CreateMeshCaptureProxy()
{
  return new FMDMeshCaptureProxy();
}

void UMDMeshCapture::DestroyMeshCaptureProxy(FMDMeshCaptureProxy* MeshCaptureProxy)
{
  delete MeshCaptureProxy;
}
