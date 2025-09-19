// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "AnimNotifyState_ModifyAnimSpeed.generated.h"

#define UE_API ARRANGER_API


UCLASS(editinlinenew, const, hidecategories = Object, collapsecategories, MinimalAPI, meta = (DisplayName = "Modify Animation Speed"))
class UAnimNotifyState_ModifyAnimSpeed : public UAnimNotifyState
{
	GENERATED_BODY()

public:
  UE_API virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
  UE_API virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
  UE_API FString GetNotifyName_Implementation() const override;

  UPROPERTY(EditAnywhere, Category = "AnimSpeedModification" , meta = (DisplayName = "Play Speed", ClampMin = 0.1))
  float AnimPlaySpeed = 1.0;

  float m_speedBeforeModify = 0.0f;
};

#undef UE_API