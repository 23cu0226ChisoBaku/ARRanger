// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "MDMeshSnapShot.h"

#include "MDMeshCapture.generated.h"

class UMeshComponent;

/**
 * 
 */
UCLASS()
class MOTIONDIFF_API UMDMeshCapture : public UObject
{
	GENERATED_BODY()

  public:
    UMDMeshCapture(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    
    void SaveMeshSnapshot(FName SnapshotName);

    virtual void CaptureMesh(UMeshComponent* MeshComponent) PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void Reset() PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void ShowSnapshots() PURE_VIRTUAL(UMDMeshCapture, ;)
    virtual void HideSnapshots() PURE_VIRTUAL(UMDMeshCapture, ;)

  protected:
    virtual void SnapshotMesh(FMDMeshSnapshot& Snapshot) PURE_VIRTUAL(UMDMeshCapture, ;)
    const FMDMeshSnapshot* GetSnapshot(FName SnapshotName) const;
    FMDMeshSnapshot& AddSnapshot(FName SnapshotName);
    void RemoveSnapshot(FName SnapshotName);
    
  private:
    TArray<FMDMeshSnapshot> m_snapshots;
};
