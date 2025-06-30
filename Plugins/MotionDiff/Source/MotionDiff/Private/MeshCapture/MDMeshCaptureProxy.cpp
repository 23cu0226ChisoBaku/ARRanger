// Fill out your copyright notice in the Description page of Project Settings.

#include "MeshCapture/MDMeshCaptureProxy.h"
#include "MeshCapture/MDMeshCapture.h"

#include "MotionDiff/MotionDiffLogChannels.h"

FMDMeshSnapshot& FMDMeshCaptureProxy::AddSnapshot(FName SnapshotName)
{
  FMDMeshSnapshot* snapshot = m_snapshots.FindByPredicate(
    [SnapshotName](const FMDMeshSnapshot& Snapshot)
    {
      return Snapshot.SnapshotName == SnapshotName;
    }
  );

  // 存在しているスナップショットを再利用
  if (snapshot != nullptr)
  {
    snapshot->Reset();
  }
  else
  {
    // 新しいスナップショットを作成
    snapshot = &m_snapshots[m_snapshots.AddDefaulted()];
  }

  snapshot->SnapshotName = SnapshotName;
  return *snapshot;
}

void FMDMeshCaptureProxy::RemoveSnapshot(FName SnapshotName)
{
  const int32 index = m_snapshots.IndexOfByPredicate(
    [SnapshotName](const FMDMeshSnapshot& Snapshot)
    {
      return Snapshot.SnapshotName == SnapshotName;
    }
  );

  if (index == INDEX_NONE)
  {
    return;
  }

  m_snapshots.RemoveAtSwap(index);
}

void FMDMeshCaptureProxy::SaveMeshSnapshot(UMDMeshCapture* MeshCapture, FName SnapshotName)
{
  check(MeshCapture != nullptr);
  if (MeshCapture == nullptr)
  {
    UE_LOG(LogMotionDiff, Warning, TEXT("Mesh capture is null. Function name: FMDMeshCaptureProxy::SaveMeshSnapshot"));
    return;
  }

  FMDMeshSnapshot& snapshot = AddSnapshot(SnapshotName);
  MeshCapture->SnapshotMesh(snapshot);
}

const FMDMeshSnapshot* FMDMeshCaptureProxy::GetSnapshot(FName SnapshotName) const
{
  return m_snapshots.FindByPredicate(
    [SnapshotName](const FMDMeshSnapshot& Snapshot)
    {
      return Snapshot.SnapshotName == SnapshotName;
    }
  );
}

const TArray<FMDMeshSnapshot>& FMDMeshCaptureProxy::GetAllSnapshots() const
{
  return m_snapshots;
}