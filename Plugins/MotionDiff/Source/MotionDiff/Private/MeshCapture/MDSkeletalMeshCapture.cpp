#include "MeshCapture/MDSkeletalMeshCapture.h"

#include "MeshCapture/MDSkeletalMeshCaptureProxy.h"
#include "MotionDiff/MotionDiffLogChannels.h"

// NOTE: For only internal use
#include "MotionDiff/Internal/MDMeshCaptureHelperLibrary.h"

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
  const auto& snapshots = proxy.GetAllSnapshots();

  for (const auto& snapshot : snapshots)
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
        
        const FMDMeshVertexBuffers& snapshotVertexBuffers = snapshot.MeshVertexBuffers;

        // FIXME: Start temporary code

        const FSkeletalMeshLODRenderData& lodRenderData = skeletalMesh->GetResourceForRendering()->LODRenderData[snapshot.LODIndex];
        
        const int32 sectionNum = lodRenderData.RenderSections.Num();
        for (int32 sectionIdx = 0; sectionIdx < sectionNum; ++sectionIdx)
        {
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
        }
      }
    }
  }
}

void UMDSkeletalMeshCapture::HideSnapshots()
{

}

void UMDSkeletalMeshCapture::ApplyMaterialOverride(UMaterialInterface* Material)
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
  const TArray<FTransform>& boneMatrices = m_skeletalMeshComp->GetComponentSpaceTransforms();

  // Vertex color
  const bool bHasVertexColor = (lodRenderData.StaticVertexBuffers.ColorVertexBuffer.GetNumVertices() > 0);

  const int32 verticesNum = lodRenderData.GetNumVertices();
  Snapshot.MeshVertexBuffers.Vertices.Reset(verticesNum);
  Snapshot.MeshVertexBuffers.Vertices.AddUninitialized(verticesNum);
  Snapshot.MeshVertexBuffers.Normals.Reset(verticesNum);
  Snapshot.MeshVertexBuffers.Normals.AddUninitialized(verticesNum);
  Snapshot.MeshVertexBuffers.Tangents.Reset(verticesNum);
  Snapshot.MeshVertexBuffers.Tangents.AddUninitialized(verticesNum);
  Snapshot.MeshVertexBuffers.Colors.Reset(verticesNum);
  Snapshot.MeshVertexBuffers.Colors.AddUninitialized(verticesNum);

  // Check uv channel nums
  const int32 uvChannelsNum = lodRenderData.GetNumTexCoords();
  const int32 snapshotSupportedUVChannelsNum = Snapshot.GetSupportedNumUVChannels();
  const int32 validUVChannelsNum = uvChannelsNum <= snapshotSupportedUVChannelsNum ? uvChannelsNum : snapshotSupportedUVChannelsNum;
  
  for (int32 i = 0; i < validUVChannelsNum; ++i)
  {
    Snapshot.MeshVertexBuffers.UVContainer.ResetByChannel(i, verticesNum);
  }

  for (int32 vertexIdx = 0; vertexIdx < verticesNum; ++vertexIdx)
  {
    FVector skinnedPos = FVector::ZeroVector;
    // TangentZ
    FVector skinnedNormal = FVector::ZeroVector;
    FVector skinnedTangentX = FVector::ZeroVector;

    const FVector originPos = static_cast<FVector>(vertexBuffer.VertexPosition(vertexIdx));
    const FVector originNormal = FVector{vertexInstanceBuffer.VertexTangentZ(vertexIdx)};
    const FVector originTangentX = FVector{vertexInstanceBuffer.VertexTangentX(vertexIdx)};

    // Use skin weight to recalculate all vertexs
    const FSkinWeightInfo skinWeights = skinBuffer->GetVertexSkinWeights(vertexIdx);
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
      const FTransform boneTransform = boneMatrices[static_cast<uint32>(boneIdx)];

      // NOTE: Transform local vertex position of bon to world position
      skinnedPos += boneTransform.TransformPosition(originPos) * realWeight;
      skinnedNormal += boneTransform.TransformVector(originNormal) * realWeight;
      skinnedTangentX += boneTransform.TransformVector(originTangentX) * realWeight;
    }

    // Copy vertex position
    Snapshot.MeshVertexBuffers.Vertices[vertexIdx] = skinnedPos;

    // Copy vertex normal
    Snapshot.MeshVertexBuffers.Normals[vertexIdx] = skinnedNormal.GetSafeNormal();

    // Use this to calculate if we should flip tangentY
    const FVector originTangentY = FVector{vertexInstanceBuffer.VertexTangentY(vertexIdx)};
    // NOTE: It must be FVector::CrossProduct(TangentZ, TangentX) in Unreal Engine
    const float sign = FVector::DotProduct(FVector::CrossProduct(originNormal, originTangentX), originTangentY);

    // Copy UVs
    for (int32 channel = 0; channel < validUVChannelsNum; ++channel)
    {
      const FVector2D uv = static_cast<FVector2D>(vertexInstanceBuffer.GetVertexUV(vertexIdx, channel));
      Snapshot.MeshVertexBuffers.UVContainer.AddUVByChannel(uv, channel);
    }

    // Copy colors
    if (bHasVertexColor)
    {
      const FColor& vertexColor = lodRenderData.StaticVertexBuffers.ColorVertexBuffer.VertexColor(vertexIdx); 
      Snapshot.MeshVertexBuffers.Colors[vertexIdx] = FLinearColor{vertexColor};
    }
    else
    {
      Snapshot.MeshVertexBuffers.Colors[vertexIdx] = FLinearColor::White;
    }

    // Copy vertex tangentX
    Snapshot.MeshVertexBuffers.Tangents[vertexIdx].TangentX = skinnedTangentX.GetSafeNormal();
    Snapshot.MeshVertexBuffers.Tangents[vertexIdx].bFlipTangentY = (sign < 0.0f);
  }

  // Copy triangles
  // NOTE: Need remapping index
  const FRawStaticIndexBuffer16or32Interface* indexBuffer = lodRenderData.MultiSizeIndexContainer.GetIndexBuffer();
  if (indexBuffer != nullptr)
  {
    const int32 indicesNum = indexBuffer->Num();
    Snapshot.MeshVertexBuffers.Triangles.Reset(indicesNum);
    Snapshot.MeshVertexBuffers.Triangles.AddUninitialized(indicesNum);

    TMap<int32, int32> meshToTriangleIdx{};

    int32 sectionNum = lodRenderData.RenderSections.Num();

    for (int32 indexID = 0; indexID < indicesNum - 2; indexID += 3)
    {
      Snapshot.MeshVertexBuffers.Triangles[indexID] = indexBuffer->Get(indexID);
      Snapshot.MeshVertexBuffers.Triangles[indexID + 1] = indexBuffer->Get(indexID + 1);
      Snapshot.MeshVertexBuffers.Triangles[indexID + 2] = indexBuffer->Get(indexID + 2);
    }
  }

  // Copy LODIndex
  Snapshot.LODIndex = validLODIdx;

  const FReferenceSkeleton& refSkeleton = skeletalMesh->GetRefSkeleton();

}

FMDMeshCaptureProxy* UMDSkeletalMeshCapture::CreateMeshCaptureProxy()
{
  return new FMDSkeletalMeshCaptureProxy();
}

void UMDSkeletalMeshCapture::DestroyMeshCaptureProxy(FMDMeshCaptureProxy* MeshCaptureProxy)
{
  delete MeshCaptureProxy;
}