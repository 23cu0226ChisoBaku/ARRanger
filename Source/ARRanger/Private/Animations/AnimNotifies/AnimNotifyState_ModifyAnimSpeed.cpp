// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/AnimNotifyState_ModifyAnimSpeed.h"

void UAnimNotifyState_ModifyAnimSpeed::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
  Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

  if (MeshComp != nullptr)
  {
    UAnimInstance* animInst = MeshComp->GetAnimInstance();
    if (animInst != nullptr)
    {
      if (UAnimMontage* montage = ::Cast<UAnimMontage>(Animation))
      {
        if (animInst->Montage_IsPlaying(montage))
        {
          // Modify play rate
          m_speedBeforeModify = animInst->Montage_GetPlayRate(montage);
          animInst->Montage_SetPlayRate(montage, AnimPlaySpeed);
        }
      }
    }
  }
}

void UAnimNotifyState_ModifyAnimSpeed::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
  if (MeshComp != nullptr)
  {
    UAnimInstance* animInst = MeshComp->GetAnimInstance();
    if (animInst != nullptr)
    {
      if (UAnimMontage* montage = ::Cast<UAnimMontage>(Animation))
      {
        if (animInst->Montage_IsPlaying(montage))
        {
          // Resume play rate
          animInst->Montage_SetPlayRate(montage, m_speedBeforeModify);
        }
      }
    }
  }
  
  Super::NotifyEnd(MeshComp, Animation, EventReference);
}
FString UAnimNotifyState_ModifyAnimSpeed::GetNotifyName_Implementation() const
{
  return FString::Printf(TEXT("Modified Play Speed : [%f]"), AnimPlaySpeed);
}
