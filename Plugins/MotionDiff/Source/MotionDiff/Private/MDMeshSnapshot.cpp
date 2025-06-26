#include "MDMeshSnapshot.h"

void FMDMeshSnapshot::Reset()
{
  MeshVertices.Reset();
  SnapshotName = NAME_None;
  bIsValid = false;
}