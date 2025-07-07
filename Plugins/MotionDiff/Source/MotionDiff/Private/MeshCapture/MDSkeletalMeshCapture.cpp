#include "MeshCapture/MDSkeletalMeshCapture.h"

#include "MeshCapture/MDSkeletalMeshCaptureProxy.h"
#include "MotionDiff/MotionDiffLogChannels.h"

// NOTE: For only internal use
#include "MotionDiff/Internal/MDMeshCaptureHelperLibrary.h"
#include "MotionDiff/Internal/MDMeshAssetCreator.h"

#include "Components/SkeletalMeshComponent.h"
#include "Rendering/SkeletalMeshRenderData.h"

// TODO
#include "ProceduralMeshComponent.h"

namespace SkeletalMeshCapture::Private
{
  constexpr float UE_SKIN_WEIGHT_NORMALIZER_UINT8 = 255.0f;
  constexpr float UE_SKIN_WEIGHT_NORMALIZER_UINT16 = 65535.0f;
}

UMDSkeletalMeshCapture::UMDSkeletalMeshCapture(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , m_skeletalMeshComp{nullptr}
{
}

void UMDSkeletalMeshCapture::CaptureMesh(UMeshComponent* MeshComponent)
{
  check(MeshComponent != nullptr);

  USkeletalMeshComponent* skeletalMeshComp = Cast<USkeletalMeshComponent>(MeshComponent);
  check(skeletalMeshComp != nullptr);
  if (skeletalMeshComp == nullptr)
  {
    UE_LOG(LogMotionDiff, Error, TEXT("Can not use a non USkeletalMeshComponent in UMDSkeletalMeshCapture. Function called:[UMDSkeletalMeshCapture::CaptureMesh]"));
  }

  m_skeletalMeshComp = skeletalMeshComp;
}

void UMDSkeletalMeshCapture::Reset()
{
  m_skeletalMeshComp = nullptr;
}

void UMDSkeletalMeshCapture::ShowSnapshots()
{
  check(m_skeletalMeshComp != nullptr);
  if (m_skeletalMeshComp == nullptr)
  {
    UE_LOG(LogMotionDiff, Error, TEXT("Capture a skeletal mesh before using [UMDSkeletalMeshCapture::ShowSnapshots]"));

    return;
  }

  const USkeletalMesh* skeletalMesh = m_skeletalMeshComp->GetSkeletalMeshAsset();
  if (skeletalMesh == nullptr)
  {
    return;
  }

  FMDMeshCaptureProxy& proxy = GetMeshCaptureProxy<FMDMeshCaptureProxy>();
  const TArray<FMDMeshSnapshot>& snapshots = proxy.GetAllSnapshots();
  const TArray<FMDMeshCaptureMaterial>& snapshotMats = GetMaterials();

  for (const FMDMeshSnapshot& snapshot : snapshots)
  {
    // create new mesh
    {
      if (UWorld* currentWorld = m_skeletalMeshComp->GetWorld())
      {
        // Set snapshot actor's transform with owner's transform of static mesh component, use FTransform::Identity if owner is null
        const FTransform& newMeshUserTransform = (m_skeletalMeshComp->GetOwner() != nullptr) ? m_skeletalMeshComp->GetOwner()->GetTransform() : FTransform::Identity; 

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

        // FIXME: Start temporary code

        const FSkeletalMeshLODRenderData& lodRenderData = skeletalMesh->GetResourceForRendering()->LODRenderData[snapshot.LODIndex];
        
        const int32 sectionNum = lodRenderData.RenderSections.Num();
        for (int32 sectionIdx = 0; sectionIdx < sectionNum; ++sectionIdx)
        {
          const FMDMeshVertexBuffers& snapshotVertexBuffers = snapshot.MeshSectionMap.GetSectionMeshVertexBuffers(sectionIdx);
          // Convert to PMC Tangent
          TArray<FProcMeshTangent> convertedTangents;
          MotionDiff::FMeshCaptureHelperLibrary::ConvertToProcMeshTangent(snapshotVertexBuffers.Tangents, convertedTangents);
          
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


          if (snapshotMats.Num() > sectionIdx)
          {
            procMeshComp->SetMaterial(sectionIdx, snapshotMats[sectionIdx].Material);
          }
        }

        newMeshActor->FinishSpawning(newMeshUserTransform);

        ::CreateStaticMeshFromPMC(procMeshComp, TEXT("/MotionDiff/"), TEXT("CheckerMesh"));
      }
    }
  }
}

void UMDSkeletalMeshCapture::HideSnapshots()
{

}

void UMDSkeletalMeshCapture::SnapshotMesh(FMDMeshSnapshot& Snapshot, const int32 LODIndex)
{
  using namespace SkeletalMeshCapture::Private;

  check(m_skeletalMeshComp != nullptr);
  if (m_skeletalMeshComp == nullptr)
  {
    UE_LOG(LogMotionDiff, Error, TEXT("Initialize before snapshot mesh.Call CaptureMesh First"));

    return;
  }

  USkeletalMesh* skeletalMesh = m_skeletalMeshComp->GetSkeletalMeshAsset();
  check(skeletalMesh != nullptr);
  if (skeletalMesh == nullptr)
  {
    UE_LOG(LogMotionDiff, Error, TEXT("There is no skeletal mesh in skeletal mesh component:[%s]"), *GetNameSafe(m_skeletalMeshComp));

    return;
  }

  const FSkeletalMeshRenderData* skeletalRenderData = skeletalMesh->GetResourceForRendering();
  if (skeletalRenderData == nullptr)
  {
    UE_LOG(LogMotionDiff, Error, TEXT("There is no render data in SkeletalMesh:[%s]"), *GetNameSafe(skeletalMesh));

    return;
  }

  // Make sure the LOD index is valid
  int32 validLODIdx = LODIndex;
  if (!skeletalRenderData->LODRenderData.IsValidIndex(validLODIdx))
  {
    validLODIdx = 0;
  }

  const FSkeletalMeshLODRenderData& lodRenderData = skeletalRenderData->LODRenderData[validLODIdx];
  const FPositionVertexBuffer& vertexBuffer = lodRenderData.StaticVertexBuffers.PositionVertexBuffer;
  const FStaticMeshVertexBuffer& vertexInstanceBuffer = lodRenderData.StaticVertexBuffers.StaticMeshVertexBuffer;

  // Skin weights
  const FSkinWeightVertexBuffer* skinBuffer = lodRenderData.GetSkinWeightVertexBuffer();
  // Bone transforms
  const TArray<FTransform>& boneComponentTransforms = m_skeletalMeshComp->GetComponentSpaceTransforms();
  // Skeleton pose
  const FReferenceSkeleton& refSkeleton = skeletalMesh->GetRefSkeleton();
  const TArray<FTransform>& refSkeletonPoses = refSkeleton.GetRefBonePose();
  const int32 bonesNum = refSkeleton.GetNum();

  // Vertex color
  const bool bHasVertexColor = (lodRenderData.StaticVertexBuffers.ColorVertexBuffer.GetNumVertices() > 0);

  // Index buffer
  const FRawStaticIndexBuffer16or32Interface* indexBuffer = lodRenderData.MultiSizeIndexContainer.GetIndexBuffer();
  if (indexBuffer != nullptr)
  {
    const int32 sectionNum = lodRenderData.RenderSections.Num();
    for (int32 sectionIdx = 0; sectionIdx < sectionNum; ++sectionIdx)
    {
      const FSkelMeshRenderSection& currentSection = lodRenderData.RenderSections[sectionIdx];
  
      // Section vertex buffers
      FMDMeshVertexBuffers& meshVertexBuffers = Snapshot.MeshSectionMap.GetSectionMeshVertexBuffers(sectionIdx);
  
      const int32 triangleVerticesNum = currentSection.NumTriangles * 3; 
      const int32 verticesNum = currentSection.GetNumVertices();
      
      meshVertexBuffers.Vertices.Reset(verticesNum);
      meshVertexBuffers.Normals.Reset(verticesNum);
      meshVertexBuffers.Tangents.Reset(verticesNum);
      meshVertexBuffers.Colors.Reset(verticesNum);
      meshVertexBuffers.Triangles.Reset(triangleVerticesNum);

      // Check uv channel nums
      const int32 uvChannelsNum = lodRenderData.GetNumTexCoords();
      const int32 snapshotSupportedUVChannelsNum = Snapshot.GetSupportedNumUVChannels();
      const int32 validUVChannelsNum = uvChannelsNum <= snapshotSupportedUVChannelsNum ? uvChannelsNum : snapshotSupportedUVChannelsNum;

      for (int32 channel = 0; channel < validUVChannelsNum; ++channel)
      {
        meshVertexBuffers.UVContainer.ResetByChannel(channel, verticesNum);
      }

      TMap<int32, int32> triangleToVertexIndexRemap{};
      TArray<FMatrix> refPoseComponentMatrices{};
      refPoseComponentMatrices.Reset(bonesNum);
      refPoseComponentMatrices.AddUninitialized(bonesNum);

      // Calculate pose component space
      // TODO Need research
      for (int32 boneIdx = 0; boneIdx < bonesNum; ++boneIdx)
      {
        const int32 parentIdx = refSkeleton.GetParentIndex(boneIdx);
        const FTransform boneLocalTransform = refSkeletonPoses[boneIdx];

        if (parentIdx != INDEX_NONE)
        {
          refPoseComponentMatrices[boneIdx] = (boneLocalTransform * FTransform(refPoseComponentMatrices[parentIdx])).ToMatrixWithScale();
        }
        else
        {
          refPoseComponentMatrices[boneIdx] = boneLocalTransform.ToMatrixWithScale();
        }
      }
  
      int32 vertexIdx = 0;
      for (int32 triangleIdx = 0; triangleIdx < triangleVerticesNum; ++triangleIdx)
      {
        // Calculate the proper vertex index in current section
        const int32 globalIdx = indexBuffer->Get(currentSection.BaseIndex + triangleIdx);

        if (!triangleToVertexIndexRemap.Contains(globalIdx))
        {
          FVector skinnedPos = FVector::ZeroVector;
          FVector skinnedTangentX = FVector::ZeroVector;
          FVector skinnedTangentY = FVector::ZeroVector;
          // TangentZ
          FVector skinnedNormal = FVector::ZeroVector;

          const FVector originPos = static_cast<FVector>(vertexBuffer.VertexPosition(globalIdx));
          const FVector originTangentX = FVector{vertexInstanceBuffer.VertexTangentX(globalIdx)};
          const FVector originTangentY = FVector{vertexInstanceBuffer.VertexTangentY(globalIdx)};
          const FVector originNormal = FVector{vertexInstanceBuffer.VertexTangentZ(globalIdx)};

          // Use skin weight to recalculate all vertexs
          const FSkinWeightInfo skinWeights = skinBuffer->GetVertexSkinWeights(globalIdx);
          for (int32 influenceIdx = 0; influenceIdx < MAX_TOTAL_INFLUENCES; ++influenceIdx)
          {
            const FBoneIndexType boneIdx = skinWeights.InfluenceBones[influenceIdx];
            const uint16 weight = skinWeights.InfluenceWeights[influenceIdx];
            if (weight == 0)
            {
              continue;
            }

            // NOTE: Inside is 16bit integer weight.Should change to normalized weight by using maximum number of uint16(65535)
            // NOTE: But skinBuffer->Use16BitBoneIndex returns false by unknown reason.
            const float normalizer = UE_SKIN_WEIGHT_NORMALIZER_UINT16;
            const float realWeight = weight / normalizer;
            // TODO: Need research
            const FMatrix& currentBoneMatrix = boneComponentTransforms[static_cast<uint32>(boneIdx)].ToMatrixWithScale(); 
            const FMatrix& InvBindMatrix = refPoseComponentMatrices[static_cast<uint32>(boneIdx)].InverseFast();

            const FMatrix& skinnedMatrix = currentBoneMatrix * InvBindMatrix;

            // NOTE: Transform local vertex position of bon to world position
            skinnedPos += skinnedMatrix.TransformPosition(originPos) * realWeight;
            skinnedTangentX += skinnedMatrix.TransformVector(originTangentX) * realWeight;
            skinnedTangentY += skinnedMatrix.TransformVector(originTangentY) * realWeight;
            skinnedNormal += skinnedMatrix.TransformVector(originNormal) * realWeight;
          }

          // Copy vertex position
          meshVertexBuffers.Vertices.Add(skinnedPos);

          // Copy vertex normal
          meshVertexBuffers.Normals.Add(skinnedNormal.GetSafeNormal());

          // Use this to calculate if we should flip tangentY
        // NOTE: It must be FVector::CrossProduct(TangentZ, TangentX) in Unreal Engine
          const float sign = FVector::DotProduct(FVector::CrossProduct(skinnedNormal, skinnedTangentX), skinnedTangentY);

          // Copy UVs
          for (int32 channel = 0; channel < validUVChannelsNum; ++channel)
          {
            const FVector2D uv = static_cast<FVector2D>(vertexInstanceBuffer.GetVertexUV(globalIdx, channel));
            meshVertexBuffers.UVContainer.AddUVByChannel(uv, channel);
          }

          // Copy colors
          if (bHasVertexColor)
          {
            const FColor& vertexColor = lodRenderData.StaticVertexBuffers.ColorVertexBuffer.VertexColor(globalIdx); 
            meshVertexBuffers.Colors.Add(FLinearColor{vertexColor});
          }
          else
          {
            meshVertexBuffers.Colors.Add(FLinearColor::White);
          }

          // Copy vertex tangentX
          meshVertexBuffers.Tangents.Add({skinnedTangentX.GetSafeNormal(), (sign < 0.0f)});

          triangleToVertexIndexRemap.Add(globalIdx, vertexIdx);
          ++vertexIdx;
        }
      }

      // Copy triangles
      for (int32 triangleIdx = 0; triangleIdx < triangleVerticesNum; ++triangleIdx)
      {
        const int32 globalIdx = indexBuffer->Get(currentSection.BaseIndex + triangleIdx);
        const int32 remappedIdx = triangleToVertexIndexRemap[globalIdx];
        meshVertexBuffers.Triangles.Add(remappedIdx);
      }
    }
  }
  // Copy LODIndex
  Snapshot.LODIndex = validLODIdx;

  Snapshot.bIsValid = true;

}

FMDMeshCaptureProxy* UMDSkeletalMeshCapture::CreateMeshCaptureProxy()
{
  return new FMDSkeletalMeshCaptureProxy();
}

void UMDSkeletalMeshCapture::DestroyMeshCaptureProxy(FMDMeshCaptureProxy* MeshCaptureProxy)
{
  delete MeshCaptureProxy;
}