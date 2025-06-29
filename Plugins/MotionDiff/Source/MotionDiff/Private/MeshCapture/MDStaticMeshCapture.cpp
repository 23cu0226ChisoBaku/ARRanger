// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshCapture/MDStaticMeshCapture.h"

#include "Components/StaticMeshComponent.h"

void UMDStaticMeshCapture::CaptureMesh(UMeshComponent* MeshComponent)
{
  check(MeshComponent != nullptr);

  UStaticMeshComponent* staticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
  check(staticMeshComp != nullptr);
  if (staticMeshComp == nullptr)
  {
    return;
  }

#if WITH_EDITOR
  EnableCPUAccess(staticMeshComp);
#endif

  m_staticMeshComp = staticMeshComp;
}

void UMDStaticMeshCapture::Reset()
{

}

void UMDStaticMeshCapture::ShowSnapshots()
{

}

void UMDStaticMeshCapture::HideSnapshots()
{

}

void UMDStaticMeshCapture::SnapshotMesh(FMDMeshSnapshot& Snapshot)
{
  Snapshot.bIsValid = false;

  check(m_staticMeshComp != nullptr); 
  if (m_staticMeshComp == nullptr)
  {
    return;
  }

  const UStaticMesh* staticMesh = m_staticMeshComp->GetStaticMesh();
  if (staticMesh == nullptr)
  {
    return;
  } 

  const FStaticMeshRenderData* meshRenderData = staticMesh->GetRenderData();
  if (meshRenderData == nullptr)
  {
    return;
  }

  // メッシュの頂点座標バッファを取得
  if (meshRenderData->LODResources.Num() > 0)
  {
    const FPositionVertexBuffer& posVertexBuffer = meshRenderData->LODResources[0].VertexBuffers.PositionVertexBuffer;
    const FRawStaticIndexBuffer& indexBuffer = meshRenderData->LODResources[0].IndexBuffer;
    check(posVertexBuffer.GetNumVertices() != indexBuffer.GetNumIndices());

    if (posVertexBuffer.IsInitialized())
    {
      const uint32 vertexNum = posVertexBuffer.GetNumVertices();
      for (uint32 i = 0; i < vertexNum; ++i)
      {
        const FVector3f localVertexPosition = posVertexBuffer.VertexPosition(i);

        FMDMeshVertexInfo& info = Snapshot.MeshVerticesInfo[i];
        info.LocalMeshVertexPosition = localVertexPosition;
        info.MeshVertexIndex = indexBuffer.GetIndex(i);

        Snapshot.bIsValid = true;
      }
    }
  }
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

