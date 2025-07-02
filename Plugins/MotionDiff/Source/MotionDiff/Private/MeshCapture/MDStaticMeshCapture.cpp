// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshCapture/MDStaticMeshCapture.h"

#include "MotionDiff/MotionDiffLogChannels.h"
#include "MeshCapture/MDMeshCaptureProxy.h"

#include "Components/StaticMeshComponent.h"
#include "StaticMeshOperations.h"
#include "StaticMeshAttributes.h"
#include "ProceduralMeshComponent.h"


#include "KismetProceduralMeshLibrary.h"    // Copy

// FIXME Debug purpose
#include "MotionDiff/Internal/MDMeshAssetCreator.h"


namespace MeshCapture::Private
{
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
}

UMDStaticMeshCapture::UMDStaticMeshCapture(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , m_staticMeshComp{nullptr}
  , m_overrideMaterial{nullptr}
{

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
  // FIXME Need implementation
  m_staticMeshComp = nullptr;
  m_overrideMaterial = nullptr;
}

void UMDStaticMeshCapture::ShowSnapshots()
{
  check(m_staticMeshComp != nullptr);
  if (m_staticMeshComp == nullptr)
  {
    UE_LOG(LogMotionDiff, Error, TEXT("Capture a static mesh before using [UMDStaticMeshCapture::ShowSnapshots]"));
    return;
  }

  FMDMeshCaptureProxy& proxy = GetMeshCaptureProxy<FMDMeshCaptureProxy>();
  const auto& snapshots = proxy.GetAllSnapshots();

  for (int32 i = 0; i < snapshots.Num(); ++i)
  {
    const auto& snapshot = snapshots[i];

    // Use this to get Section index
    // FIXME Start Temporary code
    const FMeshDescription* meshDesc = GetMeshDescription(0);
    if (meshDesc == nullptr)
    {
      return;
    }
    const UStaticMesh* staticMesh = m_staticMeshComp->GetStaticMesh();
    if (staticMesh == nullptr)
    {
      return;
    }
    // FIXME End

    // create new mesh
    {
      
      if (UWorld* currentWorld = m_staticMeshComp->GetWorld())
      {
        // Set snapshot actor's transform with owner's transform of static mesh component, use FTransform::Identity if owner is null
        const FTransform& newMeshUserTransform = (m_staticMeshComp->GetOwner() != nullptr) ? m_staticMeshComp->GetOwner()->GetTransform() : FTransform::Identity; 

        AActor* newMeshActor = currentWorld->SpawnActorDeferred<AActor>(AActor::StaticClass(), newMeshUserTransform);
        check(newMeshActor != nullptr);
        if (newMeshActor == nullptr)
        {
          // TODO Add log here

          return;
        }

        UProceduralMeshComponent* procMeshComp = NewObject<UProceduralMeshComponent>(newMeshActor);
        check(procMeshComp != nullptr);
        if (procMeshComp == nullptr)
        {
          // TODO Add log here

          return;
        }

        // Set root before register
        newMeshActor->SetRootComponent(procMeshComp);

        procMeshComp->SetRelativeTransform(newMeshUserTransform);

        // Register after root set
        procMeshComp->RegisterComponent();
        
        const FMDMeshVertexBuffers& snapshotVertexBuffers = snapshot.MeshVertexBuffers;
        
        // NOTE: Debug purpose
        {
          for (int32 channel = 0; channel < 4; ++channel)
          {
            const auto& uvs = snapshotVertexBuffers.UVContainer.GetUVsByChannel(channel);
            for (int32 j = 0; j < uvs.Num(); ++j)
            {
              const FVector2D& uv = uvs[j];
              UE_LOG(LogMotionDiff, Log, TEXT("UV after add to snapshot: Channel:[%d], U:[%f], V:[%f]"), channel, uv.X, uv.Y);
            }
          }

          AActor* testMeshActor = currentWorld->SpawnActor<AActor>(AActor::StaticClass(), FTransform::Identity);
          UProceduralMeshComponent* testPMC = NewObject<UProceduralMeshComponent>(testMeshActor);
          testMeshActor->SetRootComponent(testPMC);
          testPMC->RegisterComponent();

          UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(m_staticMeshComp, 0, testPMC, false);
        }

        // FIXME: Start temporary code

        const int32 sectionNum = staticMesh->GetNumSections(0);
        for (int32 sectionIdx = 0; sectionIdx < sectionNum; ++sectionIdx)
        {
          procMeshComp->CreateMeshSection_LinearColor(
            sectionIdx,
            snapshotVertexBuffers.Vertices,
            snapshotVertexBuffers.Triangles,
            snapshotVertexBuffers.Normals,
            snapshotVertexBuffers.UVContainer.GetUVsByChannel(0),
            snapshotVertexBuffers.UVContainer.GetUVsByChannel(1),
            snapshotVertexBuffers.UVContainer.GetUVsByChannel(2),
            snapshotVertexBuffers.UVContainer.GetUVsByChannel(3),
            snapshotVertexBuffers.Colors,
            MeshCapture::Private::ConvertTangent(snapshotVertexBuffers.Tangents),
            false
          );
        }
        // FIXME: End temporary code
        
        // Override material if allows
        if (m_overrideMaterial != nullptr)
        {
          procMeshComp->SetMaterial(0, m_overrideMaterial);
        }

        // Call this if actor spawned by SpawnActorDeferred
        newMeshActor->FinishSpawning(newMeshUserTransform);
                
        CreateStaticMeshFromPMC(procMeshComp, TEXT("/MotionDiff/"), TEXT("CheckerMesh"));
      }
    }
  }
}

void UMDStaticMeshCapture::HideSnapshots()
{
  // FIXME Need implementation
}

void UMDStaticMeshCapture::ApplyMaterialOverride(UMaterialInterface* Material)
{
  if (Material == nullptr)
  {
    UE_LOG(LogMotionDiff, Warning, TEXT("Trying to apply a null material to [%s]"), *GetNameSafe(this));

    return;
  }

  m_overrideMaterial = Material;
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
  // NOTE: Deprecated
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

  // Vertices
  const TVertexAttributesConstRef<FVector3f>& vertices = staticMeshAttribute.GetVertexPositions();

  // Triangles
  const auto& triangles = meshDesc->Triangles();
  TArray<int32> remapVertices;
  remapVertices.AddZeroed(meshDesc->Vertices().GetArraySize());

  // Vertex normals
  const auto& normals = staticMeshAttribute.GetVertexInstanceNormals();

  // UVs
  const auto& uvs = staticMeshAttribute.GetVertexInstanceUVs();
  const int32 uvInstanceNum = meshDesc->VertexInstances().Num();

  // Vertex colors
  const auto& colors = staticMeshAttribute.GetVertexInstanceColors();
  const bool bHasVertexColor = FStaticMeshOperations::HasVertexColor(*meshDesc);
  
  // Vertex tangents
  const auto& tangents = staticMeshAttribute.GetVertexInstanceTangents();
  const auto& binormalSigns = staticMeshAttribute.GetVertexInstanceBinormalSigns();
  
  const auto& vertexIDs = meshDesc->Vertices().GetElementIDs();
  const auto& vertexInstanceIDs = meshDesc->VertexInstances().GetElementIDs();

  FMDMeshVertexBuffers& vertexBuffers = Snapshot.MeshVertexBuffers;

  const int32 triangleNumber = triangles.Num() * 3;
  // TODO
  vertexBuffers.Vertices.Reset(triangleNumber);
  vertexBuffers.Vertices.AddUninitialized(triangleNumber);

  vertexBuffers.Triangles.Reset(triangleNumber);
  vertexBuffers.Triangles.AddUninitialized(triangleNumber);
  vertexBuffers.Normals.Reset(normals.GetNumElements());
  vertexBuffers.Normals.AddUninitialized(normals.GetNumElements());
  if (bHasVertexColor)
  {
    vertexBuffers.Colors.Reset(triangleNumber);
  }
  vertexBuffers.Tangents.Reset(tangents.GetNumElements());
  vertexBuffers.Tangents.AddUninitialized(tangents.GetNumElements());

  int32 vertIdx = 0;
  for (const auto& vertexID : vertexIDs)
  {
    // Prepare for remapping vertex index to triangle index
    remapVertices[vertexID.GetValue()] = vertIdx;
    
    ++vertIdx;
  }
  
  int32 vertInstanceIdx = 0;
  for (const auto& vertexInstanceID : vertexInstanceIDs)
  {
    // Copy vertex
    vertexBuffers.Vertices[vertInstanceIdx] = static_cast<FVector>(vertices[meshDesc->GetVertexInstanceVertex(vertexInstanceID)]);

    // Copy normals
    vertexBuffers.Normals[vertInstanceIdx] = static_cast<FVector>(normals[vertexInstanceID]);

    // Copy UVs
    for (int32 channel = 0; channel < uvs.GetNumChannels(); ++channel)
    {
      const FVector2D& uv = static_cast<FVector2D>(uvs.Get(vertexInstanceID, channel));
      
      // NOTE:Debug purpose
      {
        UE_LOG(LogMotionDiff, Log, TEXT("UV before add to snapshot: Channel:[%d], U:[%f], V:[%f]"), channel, uv.X, uv.Y);
      }

      vertexBuffers.UVContainer.AddUVByChannel(uv, channel);
    }

    // Copy tangents
    vertexBuffers.Tangents[vertInstanceIdx].TangentX = static_cast<FVector>(tangents[vertexInstanceID]);
    vertexBuffers.Tangents[vertInstanceIdx].bFlipTangentY = (binormalSigns[vertexInstanceID] < 0.f);

    ++vertInstanceIdx;
  }

  // Triangles
  int32 triangleIdx = 0;
  for (const auto& triangleID : triangles.GetElementIDs())
  {
    for (uint8 cornerID = 0; cornerID < 3; ++cornerID)
    {
      const FVertexInstanceID triangleVertexInstanceID = meshDesc->GetTriangleVertexInstance(triangleID, cornerID);
      
      // Copy Colors
      if (bHasVertexColor)
      {
        vertexBuffers.Colors[triangleIdx] = FLinearColor{colors[triangleVertexInstanceID]}.ToFColor(true);
      
      }

      // Copy triangles
      vertexBuffers.Triangles[triangleIdx] = remapVertices[meshDesc->GetVertexInstanceVertex(triangleVertexInstanceID).GetValue()];
      
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

FMeshDescription* UMDStaticMeshCapture::GetMeshDescription(const int32 LODIndex)
{
  return const_cast<FMeshDescription*>(static_cast<const UMDStaticMeshCapture*>(this)->GetMeshDescription(LODIndex));
}

const FMeshDescription* UMDStaticMeshCapture::GetMeshDescription(const int32 LODIndex) const
{
  check(m_staticMeshComp != nullptr);
  if (m_staticMeshComp == nullptr)
  {
    return nullptr;
  }

  const UStaticMesh* staticMesh = m_staticMeshComp->GetStaticMesh();
  check(staticMesh != nullptr);
  if (staticMesh == nullptr)
  {
    return nullptr;
  }

  return staticMesh->GetMeshDescription(LODIndex);
}

