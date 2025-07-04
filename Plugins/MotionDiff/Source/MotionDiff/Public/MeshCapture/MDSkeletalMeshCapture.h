// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeshCapture/MDMeshCapture.h"

#include "MDSkeletalMeshCapture.generated.h"

class USkeletalMeshComponent;

UCLASS()
class MOTIONDIFF_API UMDSkeletalMeshCapture : public UMDMeshCapture
{
  GENERATED_BODY()

  public:
    UMDSkeletalMeshCapture(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    virtual void CaptureMesh(UMeshComponent* MeshComponent) override;
    virtual void Reset() override;
    virtual void ShowSnapshots() override;
    virtual void HideSnapshots() override;
    virtual void ApplyMaterialOverride(UMaterialInterface* Material) override;

  protected:
    virtual void SnapshotMesh(FMDMeshSnapshot& Snapshot, const int32 LODIndex) override;

  private:
    virtual FMDMeshCaptureProxy* CreateMeshCaptureProxy();
    virtual void DestroyMeshCaptureProxy(FMDMeshCaptureProxy* MeshCaptureProxy);

  private:
    UPROPERTY()
    TObjectPtr<USkeletalMeshComponent> m_skeletalMeshComp;
};