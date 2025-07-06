#include "MDMeshSnapshot.h"

const TArray<FVector2D>& FMDMeshUVContainer::GetUVsByChannel(const int32 Channel) const
{
  // Return empty array if channel is invalid or channel uvs are not initialized 
  if (!IsChannelValid(Channel))
  {
    // An empty array
    return m_UVsArray[UV_MAX_CHANNEL_NUM];
  }

  return m_UVsArray[Channel];
}

void FMDMeshUVContainer::SetUVsByChannel(const TArray<FVector2D>& UVs, const int32 Channel)
{
  // This only support uv channel max to 8
  check(Channel >= 0 && Channel < UV_MAX_CHANNEL_NUM);
  if (Channel < 0 || Channel >= UV_MAX_CHANNEL_NUM)
  {
    return;
  }

  const int newUVsNum = UVs.Num();

  TArray<FVector2D>& channelUVArray = m_UVsArray[Channel];
  channelUVArray.Reset(newUVsNum);
  channelUVArray.AddUninitialized(newUVsNum);

  // Copy UVs
  for (int32 idx = 0; idx < newUVsNum; ++idx)
  { 
    channelUVArray[idx] = UVs[idx];
  }

}

bool FMDMeshUVContainer::IsChannelValid(const int32 Channel) const
{
  // This only support uv channel max to 8
  check(Channel >= 0 && Channel < UV_MAX_CHANNEL_NUM);
  if (Channel < 0 || Channel >= UV_MAX_CHANNEL_NUM)
  {
    return false;
  }

  return true;
}

int32 FMDMeshUVContainer::AddUVByChannel(const FVector2D& UV, const int32 Channel)
{
  // This only support uv channel max to 8
  check(Channel >= 0 && Channel < UV_MAX_CHANNEL_NUM);
  if (Channel < 0 || Channel >= UV_MAX_CHANNEL_NUM)
  {
    return INDEX_NONE;
  }

  return m_UVsArray[Channel].Emplace(UV);
}

void FMDMeshUVContainer::ResetByChannel(const int32 Channel, const int32 NewSize)
{
  // This only support uv channel max to 8
  check(Channel >= 0 && Channel < UV_MAX_CHANNEL_NUM);
  if (Channel < 0 || Channel >= UV_MAX_CHANNEL_NUM)
  {
    return;
  }

  m_UVsArray[Channel].Reset(NewSize);
  m_UVsArray[Channel].AddUninitialized(NewSize);
}

void FMDMeshUVContainer::Reset()
{
  for (int32 channel = 0; channel < UV_MAX_CHANNEL_NUM; ++channel)
  {
    m_UVsArray[channel].Reset();
  }
}


void FMDMeshVertexBuffers::Reset()
{
  Vertices.Reset();
  Triangles.Reset();
  Normals.Reset();
  UVContainer.Reset();
  Tangents.Reset();
}

void FMDMeshSnapshot::Reset()
{
  MeshVertexBuffers.Reset();
  SnapshotName = NAME_None;
  bIsValid = false;
  LODIndex = -1;
}