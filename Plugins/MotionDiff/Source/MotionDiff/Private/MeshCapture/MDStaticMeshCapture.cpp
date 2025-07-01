// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshCapture/MDStaticMeshCapture.h"

#include "MotionDiff/MotionDiffLogChannels.h"
#include "MeshCapture/MDMeshCaptureProxy.h"

#include "Components/StaticMeshComponent.h"
#include "StaticMeshOperations.h"
#include "StaticMeshAttributes.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"    // Copy

TArray<FProcMeshTangent> ConvertTangent(const TArray<FMDMeshVertexTangent>& Tangents)
{
  TArray<FProcMeshTangent> ResultTangents;
  ResultTangents.Reset(Tangents.Num());
  for (int32 i = 0; i < Tangents.Num(); ++i)
  {
    FProcMeshTangent convertedTangent{Tangents[i].TangentX, Tangents[i].bFlipTangentY};
    ResultTangents.Add(convertedTangent);
  }

  check(ResultTangents.Num() == Tangents.Num());

  return ResultTangents;
} 

void UMDStaticMeshCapture::CaptureMesh(UMeshComponent* MeshComponent)
{
  check(MeshComponent != nullptr);
  UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
  check(staticMeshComp != nullptr);

#if WITH_EDITOR
  // NOTE: It wont work if after running the program.
  // EnableCPUAccess(staticMeshComp);
#endif

  m_staticMeshComp = staticMeshComp;
}

void UMDStaticMeshCapture::Reset()
{

}

void UMDStaticMeshCapture::ShowSnapshots()
{
  FMDMeshCaptureProxy& proxy = GetMeshCaptureProxy<FMDMeshCaptureProxy>();
  const auto& snapshots = proxy.GetAllSnapshots();

  if (GEngine != nullptr)
  {
    FString log = FString::Printf(TEXT("Num:[%d]"), snapshots.Num());
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, log);
  }

  for (int32 i = 0; i < snapshots.Num(); ++i)
  {
    const auto& snapshot = snapshots[i];
    // create new mesh
    {
      if (UWorld* currentWorld = m_staticMeshComp->GetWorld())
      {
        UProceduralMeshComponent* procMeshComp = NewObject<UProceduralMeshComponent>();
        if (procMeshComp != nullptr)
        {
          const auto& snapshotVertexBuffers = snapshot.MeshVertexBuffers;
          procMeshComp->CreateMeshSection_LinearColor(
            0,
            snapshotVertexBuffers.Positions,
            snapshotVertexBuffers.Triangles,
            snapshotVertexBuffers.Normals,
            snapshotVertexBuffers.UVs0,
            snapshotVertexBuffers.UVs1,
            snapshotVertexBuffers.UVs2,
            snapshotVertexBuffers.UVs3,
            snapshotVertexBuffers.Colors,
            ConvertTangent(snapshotVertexBuffers.Tangents),
            false
          );

          AActor* newMeshActor = currentWorld->SpawnActorDeferred<AActor>(AActor::StaticClass(), m_staticMeshComp->GetOwner()->GetTransform());
          newMeshActor->SetRootComponent(procMeshComp);
          newMeshActor->FinishSpawning(m_staticMeshComp->GetOwner()->GetTransform());
        }        
      }
    }
  }
}

void UMDStaticMeshCapture::HideSnapshots()
{

}

void UMDStaticMeshCapture::SnapshotMesh(FMDMeshSnapshot& Snapshot, const int32 LODIndex)
{
  Snapshot.bIsValid = false;

  check(m_staticMeshComp != nullptr); 
  if (m_staticMeshComp == nullptr)
  {
    return;
  }

  const UStaticMesh* staticMesh = m_staticMeshComp->GetStaticMesh();
  if (staticMesh == nullptr || !staticMesh->IsValidLowLevel())
  {
    return;
  } 

  // NOTE: Use Render Data
  // NOTE: should set Static Mesh Allow CPUAccess to true
  // const FStaticMeshRenderData* meshRenderData = staticMesh->GetRenderData();
  // if (meshRenderData == nullptr)
  // {
  //   if (!staticMesh->bAllowCPUAccess)
  //   {
  //     UE_LOG(LogMotionDiff, Error, TEXT("You should set Static Mesh:[%s] Allow CPUAccess as true"), *GetNameSafe(staticMesh));
  //   } 
  //   return;
  // }

  // NOTE: Use MeshDescription
  const FMeshDescription* meshDesc = staticMesh->GetMeshDescription(LODIndex);
  if (meshDesc == nullptr)
  {
    return;
  }

  // スタティックメッシュの頂点情報を取得
  FStaticMeshConstAttributes staticMeshAttribute(*meshDesc);

  const auto& vertices = staticMeshAttribute.GetVertexPositions();
  const auto& triangles = meshDesc->Triangles();
  const auto& normals = staticMeshAttribute.GetVertexInstanceNormals();
  const auto& uvs0 = staticMeshAttribute.GetUVCoordinates(0);
  const auto& uvs1 = staticMeshAttribute.GetUVCoordinates(1);
  const auto& uvs2 = staticMeshAttribute.GetUVCoordinates(2);
  const auto& uvs3 = staticMeshAttribute.GetUVCoordinates(3);
  const auto& colors = staticMeshAttribute.GetVertexInstanceColors();
  const auto& tangents = staticMeshAttribute.GetVertexInstanceTangents();

  // Use this to set tangent
  const auto& binormalSigns = staticMeshAttribute.GetVertexInstanceBinormalSigns();
  
  const auto& vertexIDs = meshDesc->Vertices().GetElementIDs();
  const auto& vertexInstanceIDs = meshDesc->VertexInstances().GetElementIDs();

  FMDMeshVertexBuffers& vertexBuffers = Snapshot.MeshVertexBuffers;

  vertexBuffers.Positions.Reset(vertices.GetNumElements());
  vertexBuffers.Positions.AddUninitialized(vertices.GetNumElements());
  vertexBuffers.Triangles.Reset(triangles.Num() * 3);
  vertexBuffers.Triangles.AddUninitialized(triangles.Num() * 3);
  vertexBuffers.Normals.Reset(normals.GetNumElements());
  vertexBuffers.Normals.AddUninitialized(normals.GetNumElements());
  vertexBuffers.UVs0.Reset(uvs0.GetNumElements());
  vertexBuffers.UVs0.AddUninitialized(uvs0.GetNumElements());
  vertexBuffers.UVs1.Reset(uvs1.GetNumElements());
  vertexBuffers.UVs1.AddUninitialized(uvs1.GetNumElements());
  vertexBuffers.UVs2.Reset(uvs2.GetNumElements());
  vertexBuffers.UVs2.AddUninitialized(uvs2.GetNumElements());
  vertexBuffers.UVs3.Reset(uvs3.GetNumElements());
  vertexBuffers.UVs3.AddUninitialized(uvs3.GetNumElements());
  vertexBuffers.Tangents.Reset(tangents.GetNumElements());
  vertexBuffers.Tangents.AddUninitialized(tangents.GetNumElements());

  uint32 vertIdx = 0;
  const bool bHasVertexColor = FStaticMeshOperations::HasVertexColor(*meshDesc);
  for (const auto& vertexID : vertexIDs)
  {
    // Copy Vertex
    vertexBuffers.Positions[vertIdx] = static_cast<FVector>(vertices[vertexID]);


    ++vertIdx;
  }

  uint32 vertInstanceIdx = 0;
  for (const auto& vertexInstanceID : vertexInstanceIDs)
  {
    // Copy normals
    vertexBuffers.Normals[vertInstanceIdx] = static_cast<FVector>(normals[vertexInstanceID]);

    // Copy UVs
    vertexBuffers.UVs0[vertInstanceIdx] = static_cast<FVector2D>(uvs0.Get(vertexInstanceID));
    vertexBuffers.UVs1[vertInstanceIdx] = static_cast<FVector2D>(uvs1.Get(vertexInstanceID));
    vertexBuffers.UVs2[vertInstanceIdx] = static_cast<FVector2D>(uvs2.Get(vertexInstanceID));
    vertexBuffers.UVs3[vertInstanceIdx] = static_cast<FVector2D>(uvs3.Get(vertexInstanceID));

    // Copy tangents
    vertexBuffers.Tangents[vertInstanceIdx].TangentX = static_cast<FVector>(tangents[vertexInstanceID]);
    vertexBuffers.Tangents[vertInstanceIdx].bFlipTangentY = (binormalSigns[vertexInstanceID] < 0.f);

   ++vertInstanceIdx;
  }

  // Triangles
  uint32 triangleIdx = 0;
  for (const auto& triangleID : triangles.GetElementIDs())
  {
    for (uint8 cornerID = 0; cornerID < 3; ++cornerID)
    {
      // Copy triangles
      FVertexInstanceID triangleVertexInstanceID = meshDesc->GetTriangleVertexInstance(triangleID, cornerID);
      vertexBuffers.Triangles[triangleIdx] = meshDesc->GetVertexInstanceVertex(triangleVertexInstanceID).GetValue();
      
      ++triangleIdx;
    }
  }

  Snapshot.bIsValid = true;
}

// Editor Only
#if WITH_EDITOR
void UMDStaticMeshCapture::EnableCPUAccess(UStaticMeshComponent* StaticMeshComponent)
{
  checkSlow(StaticMeshComponent != nullptr);
  
  UStaticMesh* staticMesh = m_staticMeshComp->GetStaticMesh();
  checkSlow(staticMesh != nullptr);

  // Allow CPU Access to get RenderData non-null
  if (!staticMesh->bAllowCPUAccess)
  {
    staticMesh->bAllowCPUAccess = true;

    // Rebuild
    // TODO Need research
    staticMesh->Modify();
    staticMesh->MarkPackageDirty();
    staticMesh->Build(false);
    staticMesh->PostEditChange();
  }
}
#endif

