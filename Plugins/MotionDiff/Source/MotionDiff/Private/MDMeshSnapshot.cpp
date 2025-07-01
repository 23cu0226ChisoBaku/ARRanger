#include "MDMeshSnapshot.h"

void FMDMeshVertexBuffers::Reset()
{
  Positions.Reset();
  Triangles.Reset();
  Normals.Reset();
  UVs0.Reset();
  UVs1.Reset();
  UVs2.Reset();
  UVs3.Reset();
  Tangents.Reset();
}

void FMDMeshSnapshot::Reset()
{
  MeshVertexBuffers.Reset();
  SnapshotName = NAME_None;
  bIsValid = false;
  LODIndex = -1;
}