// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshCapture/MDStaticMeshCapture.h"

#include "MotionDiff/MotionDiffLogChannels.h"
#include "MeshCapture/MDMeshCaptureProxy.h"

#include "Components/StaticMeshComponent.h"

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
  const auto& snapShots = proxy.GetAllSnapshots();

  for (int32 i = 0; i < snapShots.Num(); ++i)
  {
    const auto& snapshot = snapShots[i];
    if (GEngine != nullptr)
    {
      FString Log = FString::Printf(TEXT("Snapshot info: Vertices Num:[%d], Name:[%s]"), snapshot.MeshVerticesInfo.Num(), *snapshot.SnapshotName.ToString());
      GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, Log);
    }
  }
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
    if (!staticMesh->bAllowCPUAccess)
    {
      UE_LOG(LogMotionDiff, Error, TEXT("You should set Static Mesh:[%s] Allow CPUAccess as true"), *GetNameSafe(staticMesh));
    } 
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

      // TODO Need research
      Snapshot.MeshVerticesInfo.Reset(vertexNum);
      Snapshot.MeshVerticesInfo.AddUninitialized(vertexNum);

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

