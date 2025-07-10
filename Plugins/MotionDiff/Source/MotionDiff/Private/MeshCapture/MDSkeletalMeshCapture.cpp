#include "MeshCapture/MDSkeletalMeshCapture.h"

#include "MeshCapture/MDSkeletalMeshCaptureProxy.h"
#include "MotionDiff/MotionDiffLogChannels.h"

// NOTE: For only internal use
#include "Internal/MDMeshAssetCreator.h"
#include "Internal/MDMeshGenerator.h"

#include "Components/SkeletalMeshComponent.h"
#include "Rendering/SkeletalMeshRenderData.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(MDSkeletalMeshCapture)

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

  const FSkeletalMeshRenderData* renderData = skeletalMesh->GetResourceForRendering();
  if (renderData == nullptr)
  {
    // TODO: Add Log

    return;
  }

  const int32 LODDataNum = renderData->LODRenderData.Num();
  if (LODDataNum == 0)
  {
    // TODO: Add Log

    return;
  }


  FMDMeshCaptureProxy& proxy = GetMeshCaptureProxy<FMDMeshCaptureProxy>();
  const TArray<FMDMeshSnapshot>& snapshots = proxy.GetAllSnapshots();
  const TArray<FMDMeshCaptureMaterial>& snapshotMats = GetMaterials();
  
  TArray<UMaterialInterface*> mats{};
  mats.Reset(snapshotMats.Num());
  for (int32 matIdx = 0; matIdx < snapshotMats.Num(); ++matIdx)
  {
    mats.Add(snapshotMats[matIdx].Material);
  }

  for (const FMDMeshSnapshot& snapshot : snapshots)
  {
    // Ignore invalid LODIndex
    // Currently we dont fall back to LOD0
    if (LODDataNum <= snapshot.LODIndex)
    {
      continue;
    }

    const FSkeletalMeshLODRenderData& lodRenderData = renderData->LODRenderData[snapshot.LODIndex];
    const int32 sectionNum = lodRenderData.RenderSections.Num(); 

    const float startTime = FPlatformTime::Seconds();
  
    AActor* newMeshActor = MotionDiff::FMDMeshGenerator::GeneratePMCActorBySnapshot(m_skeletalMeshComp->GetWorld(), snapshot, sectionNum, mats);

    const float deltaTime = FPlatformTime::Seconds() - startTime;

    UE_LOG(LogMotionDiff, Error, TEXT("Create mesh delta time: [%f]"), deltaTime);

    if (newMeshActor != nullptr)
    {
      m_snapshotActors.Emplace(newMeshActor);
    } 
  }
}

void UMDSkeletalMeshCapture::HideSnapshots()
{
  for (const TObjectPtr<AActor>& snapshotActor : m_snapshotActors)
  {
    if (snapshotActor != nullptr)
    {
      snapshotActor->Destroy();
    }
  }
}

FString UMDSkeletalMeshCapture::GetCaptureName() const
{
  return TEXT("SkeletalMesh_Capture");
}

void UMDSkeletalMeshCapture::RefreshMeshState()
{
  // FIXME: For test purpose
  m_skeletalMeshComp->TickAnimation(FApp::GetDeltaTime(), false);
  m_skeletalMeshComp->RefreshBoneTransforms();
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
      refPoseComponentMatrices.SetNum(bonesNum);

      // Calculate pose component space
      // TODO Need research
      for (int32 boneIdx = 0; boneIdx < bonesNum; ++boneIdx)
      {
        // NOTE: Transform
        const int32 parentIdx = refSkeleton.GetParentIndex(boneIdx);
        const FTransform& boneLocalTransform = refSkeletonPoses[boneIdx];

        if (parentIdx != INDEX_NONE)
        {
          refPoseComponentMatrices[boneIdx] = boneLocalTransform.ToMatrixWithScale() * refPoseComponentMatrices[parentIdx];
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
          // スキンウェイトを計算し、頂点に入れる
          const FSkinWeightInfo skinWeights = skinBuffer->GetVertexSkinWeights(globalIdx);
          for (int32 influenceIdx = 0; influenceIdx < MAX_TOTAL_INFLUENCES; ++influenceIdx)
          {
            const FBoneIndexType boneIdx = skinWeights.InfluenceBones[influenceIdx];
            const uint16 weight = skinWeights.InfluenceWeights[influenceIdx];
            if (weight == 0)
            {
              continue;
            }

            // NOTE: Inside is 16bit unsigned integer weight.Should change to normalized weight by using maximum number of uint16(65535)
            // NOTE: But skinBuffer->Use16BitBoneIndex returns false by unknown reason.
            const float normalizer = UE_SKIN_WEIGHT_NORMALIZER_UINT16;
            const float realWeight = weight / normalizer;
            // TODO: Need research

            const int32 skeletonBoneIdx = currentSection.BoneMap[boneIdx];

            // 骨をワールド座標原点に戻して、スキンウェイトを頂点に掛ける
            const FMatrix& currentBoneMatrix = boneComponentTransforms[static_cast<uint32>(skeletonBoneIdx)].ToMatrixWithScale(); 
            const FMatrix& InvBindMatrix = refPoseComponentMatrices[static_cast<uint32>(skeletonBoneIdx)].InverseFast();

            const FMatrix& skinnedMatrix = InvBindMatrix * currentBoneMatrix;

            // NOTE: Transform local vertex position of bone to world position
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

  Snapshot.MeshTransform = m_skeletalMeshComp->GetComponentTransform();

}

FMDMeshCaptureProxy* UMDSkeletalMeshCapture::CreateMeshCaptureProxy()
{
  return new FMDSkeletalMeshCaptureProxy();
}

void UMDSkeletalMeshCapture::DestroyMeshCaptureProxy(FMDMeshCaptureProxy* MeshCaptureProxy)
{
  delete MeshCaptureProxy;
}