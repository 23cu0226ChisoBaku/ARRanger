// Fill out your copyright notice in the Description page of Project Settings.

#include "MeshCapture/MDMeshCapture.h"
#include "MDMeshSnapshot.h"


UMDMeshCapture::UMDMeshCapture(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , m_snapshots{}
{

}

void UMDMeshCapture::SaveMeshSnapshot(FName SnapshotName)
{
  FMDMeshSnapshot& snapshot = AddSnapshot(SnapshotName);
  SnapshotMesh(snapshot);
}

const FMDMeshSnapshot* UMDMeshCapture::GetSnapshot(FName SnapshotName) const
{
  return m_snapshots.FindByPredicate(
    [SnapshotName](const FMDMeshSnapshot& Snapshot)
    {
      return Snapshot.SnapshotName == SnapshotName;
    }
  );
}

FMDMeshSnapshot& UMDMeshCapture::AddSnapshot(FName SnapshotName)
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

void UMDMeshCapture::RemoveSnapshot(FName SnapshotName)
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