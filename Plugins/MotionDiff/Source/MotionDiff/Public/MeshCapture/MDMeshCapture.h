// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "MDMeshSnapShot.h"
#include "MDMeshCaptureMaterial.h"
#include "Misc/MDConcepts.h"

#include "MDMeshCapture.generated.h"

class UMeshComponent;
class FMDMeshCaptureProxy;

UCLASS()
class MOTIONDIFF_API UMDMeshCapture : public UObject
{
	GENERATED_BODY()

  public:
    UMDMeshCapture(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // UObject interface
    virtual void BeginDestroy() override;
    
    void SaveMeshSnapshot(FName SnapshotName);
    void SetMaterials(const TArray<FMDMeshCaptureMaterial>& Materials);
    const TArray<FMDMeshCaptureMaterial>& GetMaterials() const;

    virtual void CaptureMesh(UMeshComponent* MeshComponent) PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void Reset() PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void ShowSnapshots() PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void HideSnapshots() PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void SnapshotMesh(FMDMeshSnapshot& Snapshot, const int32 LODIndex = 0) PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual FString GetCaptureName() const;

  protected:
    template<MotionDiff::Concepts::MeshCaptureProxyConcept MeshCaptureProxyType>
    MeshCaptureProxyType& GetMeshCaptureProxy();

    template<MotionDiff::Concepts::MeshCaptureProxyConcept MeshCaptureProxyType>
    const MeshCaptureProxyType& GetMeshCaptureProxy() const;

  public:
    template<MotionDiff::Concepts::MeshCaptureProxyConcept MeshCaptureProxyType>
    static MeshCaptureProxyType* GetMeshCaptureProxyStatic(UMDMeshCapture* MeshCapture);

  private:
    virtual FMDMeshCaptureProxy* CreateMeshCaptureProxy();
    virtual void DestroyMeshCaptureProxy(FMDMeshCaptureProxy* MeshCaptureProxy);

  private:

    // Dangerous raw pointer
    FMDMeshCaptureProxy* m_meshCaptureProxy;

    TArray<FMDMeshCaptureMaterial> m_materials;
};

template<MotionDiff::Concepts::MeshCaptureProxyConcept MeshCaptureProxyType>
MeshCaptureProxyType& UMDMeshCapture::GetMeshCaptureProxy()
{
  return *GetMeshCaptureProxyStatic<MeshCaptureProxyType>(this);
}

template<MotionDiff::Concepts::MeshCaptureProxyConcept MeshCaptureProxyType>
const MeshCaptureProxyType& UMDMeshCapture::GetMeshCaptureProxy() const
{
  return *GetMeshCaptureProxyStatic<const MeshCaptureProxyType>(const_cast<UMDMeshCapture*>(this));
}

template<MotionDiff::Concepts::MeshCaptureProxyConcept MeshCaptureProxyType>
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


