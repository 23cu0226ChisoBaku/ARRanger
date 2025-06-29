#include "MDMeshSnapshot.h"

void FMDMeshSnapshot::Reset()
{
  MeshVerticesInfo.Reset();
  SnapshotName = NAME_None;
  bIsValid = false;
}