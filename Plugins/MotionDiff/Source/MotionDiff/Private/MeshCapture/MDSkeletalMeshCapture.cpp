#include "MeshCapture/MDSkeletalMeshCapture.h"

#include "Components/SkeletalMeshComponent.h"
#include "MotionDiff/MotionDiffLogChannels.h"

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

}

void UMDSkeletalMeshCapture::HideSnapshots()
{

}

void UMDSkeletalMeshCapture::ApplyMaterialOverride(UMaterialInterface* Material)
{

}

void UMDSkeletalMeshCapture::SnapshotMesh(FMDMeshSnapshot& Snapshot, const int32 LODIndex)
{
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
    UE_LOG(LogMotionDiff, Error, TEXT("There is no skeletal mesh in skeletal mesh component"));

    return;
  }

  const FSkeletalMeshRenderData* skeletalRenderData = skeletalMesh->GetResourceForRendering();
}