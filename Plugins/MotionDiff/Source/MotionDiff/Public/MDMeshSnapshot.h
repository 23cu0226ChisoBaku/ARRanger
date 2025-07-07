// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MDMeshSnapshot.generated.h"

struct FMDMeshVertexTangent
{
  FVector TangentX;
  bool bFlipTangentY;
};

struct FMDMeshUVContainer
{
  public:
    FMDMeshUVContainer() = default;
    ~FMDMeshUVContainer() = default;

    const TArray<FVector2D>& GetUVsByChannel(const int32 Channel) const;
    void SetUVsByChannel(const TArray<FVector2D>& UVs, const int32 Channel);
    bool IsChannelValid(const int32 Channel) const;
    int32 AddUVByChannel(const FVector2D& UV, const int32 Channel);
    void ResetByChannel(const int32 Channel, const int32 NewSize = 0);
    void Reset();

    int32 GetSupportedNumUVChannels() const 
    {
      return static_cast<int32>(UV_MAX_CHANNEL_NUM);
    }

  private:
    enum
    {
      UV_MAX_CHANNEL_NUM = 8,
    };

    // Make the 8th channel as an empty array
    TArray<FVector2D> m_UVsArray[UV_MAX_CHANNEL_NUM + 1];

};

struct FMDMeshVertexBuffers
{

  void Reset();

  // Vertex position
  TArray<FVector> Vertices;

  // Triangle index
  TArray<int32> Triangles;

  // Normal 
  TArray<FVector> Normals;

  // UVContainer
  FMDMeshUVContainer UVContainer;

  // Color
  TArray<FLinearColor> Colors;

  // Tangent
  TArray<FMDMeshVertexTangent> Tangents;

};

// FIXME: Next start from here
struct FMDMeshSectionMap
{
  FMDMeshVertexBuffers& GetSectionMeshVertexBuffers(const int32 Section);
  const FMDMeshVertexBuffers& GetSectionMeshVertexBuffers(const int32 Section) const;
  bool HasSection(const int32 Section) const;
  void Reset();

  private:
    mutable TMap<int32, FMDMeshVertexBuffers> m_sectionMapData;
};

USTRUCT(BlueprintType)
struct MOTIONDIFF_API FMDMeshSnapshot
{
  GENERATED_BODY()

  void Reset();

  // TODO Add section version(PMC sections == MeshDescription.PolygonGroups())
  FMDMeshSectionMap MeshSectionMap;

  UPROPERTY(VisibleAnywhere)
  FName SnapshotName;

  UPROPERTY(VisibleAnywhere)
  bool bIsValid;

  UPROPERTY(VisibleAnywhere)
  int8 LODIndex;              // 無効値として-1を入れるようにint8を使用

  int32 GetSupportedNumUVChannels() const
  {
    return MeshSectionMap.GetSectionMeshVertexBuffers(0).UVContainer.GetSupportedNumUVChannels();
  }
};
