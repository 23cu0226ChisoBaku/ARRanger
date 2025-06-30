// Fill out your copyright notice in the Description page of Project Settings.

#include "MDMeshSnapShot.h"

class UMDMeshCapture;

class FMDMeshCaptureProxy
{
  public:
    FMDMeshCaptureProxy() = default;
    virtual ~FMDMeshCaptureProxy() = default;

    virtual FMDMeshSnapshot& AddSnapshot(FName SnapshotName);
    virtual void RemoveSnapshot(FName SnapshotName);
    virtual void SaveMeshSnapshot(UMDMeshCapture* MeshCapture, FName SnapshotName);
    virtual const FMDMeshSnapshot* GetSnapshot(FName SnapshotName) const;
    virtual const TArray<FMDMeshSnapshot>& GetAllSnapshots() const;

  private:
    TArray<FMDMeshSnapshot> m_snapshots;
};