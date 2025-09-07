// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/AnimNotifyState_DetectRange.h"

#include "RangeDetector/Core/PrimitiveDetectorData.h"

void UAnimNotifyState_DetectRange::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
  Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

  if (ValidateParameters(MeshComp))
  {

  }
}

void UAnimNotifyState_DetectRange::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
  Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_DetectRange::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
  Super::NotifyEnd(MeshComp, Animation, EventReference);
}

FString UAnimNotifyState_DetectRange::GetNotifyName_Implementation() const
{
  if (RangeData != nullptr)
  {
    return RangeData->GetName();
  }

  return Super::GetNotifyName_Implementation();
}

bool UAnimNotifyState_DetectRange::ValidateParameters(USkeletalMeshComponent* MeshComp) const
{
  bool bValid = true;

  if (RangeData == nullptr)
  {
    bValid = false;
  }
  else if (!MeshComp->DoesSocketExist(SocketName) && MeshComp->GetBoneIndex(SocketName) == INDEX_NONE)
  {
    bValid = false;
  }

  return bValid;
}