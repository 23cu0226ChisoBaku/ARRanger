#include "MDMeshGenerator.h"

#include "MDMeshSnapshot.h"
#include "MDMeshCaptureHelperLibrary.h"
#include "MotionDiffLogChannels.h"

#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"


namespace MotionDiff
{
  AActor* FMDMeshGenerator::GeneratePMCActorBySnapshot(UWorld* World, const FMDMeshSnapshot& Snapshot, const int32 SectionNum, const TArray<UMaterialInterface*>& Materials)
  {
    if (World == nullptr)
    {
      UE_LOG(LogMotionDiff, Warning, TEXT("World is NULL. Function called:[MotionDiff::FMDMeshGenerator::GeneratePMCactorBySnapshot]"));

      return nullptr;
    }

    if (!Snapshot.bIsValid)
    {
      UE_LOG(LogMotionDiff, Warning, TEXT("Target snapshot is invalid. Function called:[MotionDiff::FMDMeshGenerator::GeneratePMCactorBySnapshot]"));

      return nullptr;
    }


    // Set snapshot actor's transform with owner's transform of static mesh component, use FTransform::Identity if owner is null
    const FTransform& newMeshUserTransform = Snapshot.MeshTransform; 

    AActor* newMeshActor = World->SpawnActorDeferred<AActor>(AActor::StaticClass(), newMeshUserTransform);
    check(newMeshActor != nullptr);
    if (newMeshActor == nullptr)
    {
      // TODO Add log here

      return nullptr;
    }

    UProceduralMeshComponent* procMeshComp = NewObject<UProceduralMeshComponent>(newMeshActor);
    check(procMeshComp != nullptr);
    if (procMeshComp == nullptr)
    {
      // TODO Add log here

      return nullptr;
    }

    // Set root before register
    newMeshActor->SetRootComponent(procMeshComp);

    procMeshComp->SetRelativeTransform(newMeshUserTransform);

    // Register after root set
    procMeshComp->RegisterComponent();

    for (int32 sectionIdx = 0; sectionIdx < SectionNum; ++sectionIdx)
    {
      const FMDMeshVertexBuffers& snapshotVertexBuffers = Snapshot.MeshSectionMap.GetSectionMeshVertexBuffers(sectionIdx);
      // Convert to PMC Tangent
      TArray<FProcMeshTangent> convertedTangents;
      FMeshCaptureHelperLibrary::ConvertToProcMeshTangent(snapshotVertexBuffers.Tangents, convertedTangents);
      
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
        convertedTangents,
        false
      );

      if (Materials.Num() > 0)
      {
        if (Materials.Num() > sectionIdx)
        {
          procMeshComp->SetMaterial(sectionIdx, Materials[sectionIdx]);
        }
        // Fallback to use material in index 0
        else
        {
          procMeshComp->SetMaterial(sectionIdx, Materials[0]);
        }
      }
    }
    newMeshActor->FinishSpawning(newMeshUserTransform);
    
    return newMeshActor;
  }

  AActor* FMDMeshGenerator::GenerateStaticMeshActorBySnapshot(UWorld* World, const FMDMeshSnapshot& Snapshot, const int32 SectionNum, const TArray<UMaterialInterface*>& Materials)
  {
    if (World == nullptr)
    {
      UE_LOG(LogMotionDiff, Warning, TEXT("World is NULL. Function called:[MotionDiff::FMDMeshGenerator::GenerateStaticMeshActorBySnapshot]"));

      return nullptr;
    }

    if (!Snapshot.bIsValid)
    {
      UE_LOG(LogMotionDiff, Warning, TEXT("Target snapshot is invalid. Function called:[MotionDiff::FMDMeshGenerator::GenerateStaticMeshActorBySnapshot]"));

      return nullptr;
    }

     // Set snapshot actor's transform with owner's transform of static mesh component, use FTransform::Identity if owner is null
    const FTransform& newMeshUserTransform = Snapshot.MeshTransform; 

    AActor* newMeshActor = World->SpawnActorDeferred<AActor>(AActor::StaticClass(), newMeshUserTransform);
    check(newMeshActor != nullptr);
    if (newMeshActor == nullptr)
    {
      // TODO Add log here

      return nullptr;
    }

    // Create static mesh component
    UStaticMeshComponent* smc = NewObject<UStaticMeshComponent>(newMeshActor);
    check(smc != nullptr)
    if (smc == nullptr)
    {
      // TODO Add log here

      return nullptr;
    }

    newMeshActor->SetRootComponent(smc);

    smc->SetRelativeTransform(newMeshUserTransform);

    smc->RegisterComponent();

    // Start creating Static mesh
    UStaticMesh* newMesh = NewObject<UStaticMesh>(GetTransientPackage(), NAME_None, RF_Public | RF_Standalone);
    newMesh->InitResources();

    if (newMesh->GetNumSourceModels() == 0)
    {
      newMesh->AddSourceModel();
    }

    FMeshDescription* meshDesc = newMesh->GetMeshDescription(0);
    if (meshDesc == nullptr)
    {
      meshDesc = newMesh->CreateMeshDescription(0);
    }
    FStaticMeshAttributes smAttribute(*meshDesc);
    smAttribute.Register();

    const int32 sectionsNum = Snapshot.MeshSectionMap.GetSectionNum();
    for (int32 sectionIdx = 0; sectionIdx < sectionsNum; ++sectionIdx)
    {
      const FMDMeshVertexBuffers& sectionVertexBuffers = Snapshot.MeshSectionMap.GetSectionMeshVertexBuffers(sectionIdx);
    
      TMap<int32, FVertexID> vertexIDMap;
      
      // Copy vertex position
      for (int32 i = 0; i < sectionVertexBuffers.Vertices.Num(); ++i)
      {
        const FVector& Pos = sectionVertexBuffers.Vertices[i];
        FVertexID VertexID = meshDesc->CreateVertex();
        smAttribute.GetVertexPositions()[VertexID] = FVector3f{Pos};
        vertexIDMap.Add(i, VertexID);
      }
    
      // Polygon group(same as triangle)
      FPolygonGroupID polygonGroupID = meshDesc->CreatePolygonGroup();
    
      // Copy triangle
      for (int32 i = 0; i < sectionVertexBuffers.Triangles.Num(); i += 3)
      {
        TArray<FVertexInstanceID> instanceIDs{};
        for (int j = 0; j < 3; ++j)
        {
          const int32 vertIndex = sectionVertexBuffers.Triangles[i + j];
          FVertexID vertID = vertexIDMap[vertIndex];
          FVertexInstanceID instanceID = meshDesc->CreateVertexInstance(vertID);
    
          // Copy UVs
          const int32 validUVChannelNum = sectionVertexBuffers.UVContainer.GetValidChannelNum();
          for (int32 channel = 0; channel < validUVChannelNum; ++channel)
          {
            const FVector2D& uv = sectionVertexBuffers.UVContainer[channel][vertIndex];
            smAttribute.GetVertexInstanceUVs().Set(instanceID, channel, FVector2f{uv});
          }
    
          // Copy normals
          smAttribute.GetVertexInstanceNormals()[instanceID] = FVector3f(sectionVertexBuffers.Normals[vertIndex]);
    
          // TODO Copy tangents and color
    
          instanceIDs.Add(instanceID);
        }
    
        // Create polygon
        meshDesc->CreatePolygon(polygonGroupID, instanceIDs);
      }
    
    }

    // Apply materials
    for (int32 matIdx = 0; matIdx < Materials.Num(); ++matIdx)
    {
      newMesh->SetMaterial(matIdx, Materials[matIdx]);
    }
    
    // Bind mesh description to static mesh
    newMesh->CommitMeshDescription(0);
    newMesh->Build(false); 
    newMesh->MarkPackageDirty();

    smc->SetStaticMesh(newMesh);
    smc->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Finish actor spawn
    newMeshActor->FinishSpawning(newMeshUserTransform);

    return newMeshActor;
  }
}