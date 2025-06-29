// Fill out your copyright notice in the Description page of Project Settings.

#include "MDMeshSnapShot.h"

class FMDMeshCaptureProxy
{
  public:
    const FMDMeshSnapshot* GetSnapshot(FName SnapshotName) const;
    FMDMeshSnapshot& AddSnapshot(FName SnapshotName);
    void RemoveSnapshot(FName SnapshotName);

  private:
    TArray<FMDMeshSnapshot> m_snapshots;
};