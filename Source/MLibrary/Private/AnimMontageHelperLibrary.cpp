// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimMontageHelperLibrary.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

float UAnimMontageHelperLibrary::GetSectionDuration(UAnimInstance* animInstance, UAnimMontage* animMontage, FName sectionName)
{
  if (!::IsValid(animInstance) || !::IsValid(animMontage))
  {
    return 0.0f;
  }

  if (!animMontage->IsValidSectionName(sectionName))
  {
    return 0.0f;
  }

  const int32 sectionIdx = animMontage->GetSectionIndex(sectionName);
  if (sectionIdx == INDEX_NONE)
  {
    return 0.0f;
  }
  
  return animMontage->GetSectionLength(sectionIdx);
}