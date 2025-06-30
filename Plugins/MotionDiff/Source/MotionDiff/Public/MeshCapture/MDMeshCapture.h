// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "MDMeshSnapShot.h"

#include "MDMeshCapture.generated.h"

class UMeshComponent;
class FMDMeshCaptureProxy;

/**
 * 
 */
UCLASS()
class MOTIONDIFF_API UMDMeshCapture : public UObject
{
	GENERATED_BODY()

  public:
    UMDMeshCapture(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UObject interface
    virtual void BeginDestroy() override;
    
    void SaveMeshSnapshot(FName SnapshotName);

    virtual void CaptureMesh(UMeshComponent* MeshComponent) PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void Reset() PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void ShowSnapshots() PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void HideSnapshots() PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void SnapshotMesh(FMDMeshSnapshot& Snapshot) PURE_VIRTUAL(UMDMeshCapture, ;)

  protected:
    template<typename MeshCaptureProxyType>
    MeshCaptureProxyType& GetMeshCaptureProxy();

    template<typename MeshCaptureProxyType>
    const MeshCaptureProxyType& GetMeshCaptureProxy() const;

    template<typename MeshCaptureProxyType>
    static MeshCaptureProxyType* GetMeshCaptureProxyStatic(UMDMeshCapture* MeshCapture);

  private:
    virtual FMDMeshCaptureProxy* CreateMeshCaptureProxy();
    virtual void DestroyMeshCaptureProxy(FMDMeshCaptureProxy* MeshCaptureProxy);

  private:

    // Dangerous raw pointer
    FMDMeshCaptureProxy* m_meshCaptureProxy;
};

template<typename MeshCaptureProxyType>
MeshCaptureProxyType& UMDMeshCapture::GetMeshCaptureProxy()
{
  return *GetMeshCaptureProxyStatic<MeshCaptureProxyType>(this);
}

template<typename MeshCaptureProxyType>
const MeshCaptureProxyType& UMDMeshCapture::GetMeshCaptureProxy() const
{
  return *GetMeshCaptureProxyStatic<const MeshCaptureProxyType>(const_cast<UMDMeshCapture*>(this));
}

template<typename MeshCaptureProxyType>
static MeshCaptureProxyType* UMDMeshCapture::GetMeshCaptureProxyStatic(UMDMeshCapture* MeshCapture)
{
  if (MeshCapture == nullptr)
  {
    return nullptr;
  }

  if (MeshCapture->m_meshCaptureProxy == nullptr)
  {
    MeshCapture->m_meshCaptureProxy = MeshCapture->CreateMeshCaptureProxy();
  }

  return static_cast<MeshCaptureProxyType*>(MeshCapture->m_meshCaptureProxy);
}


