// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "AnimNotifyState_DetectRange.generated.h"

class UPrimitiveDetectorData;
class UGameplayAbility;

#define UE_API ARRANGER_API

DECLARE_DELEGATE_TwoParams(FOnDetectionNotify, USkeletalMeshComponent*, UAnimSequenceBase*);

UENUM()
enum struct EANS_DetectRange_NotifyDetectionType : uint8
{
  NotifyOnce UMETA(DisplayName = "Notify Only Once"),
  NotifyDuringHit UMETA(DisplayName = "Notify Multiple Times"),
};

UENUM()
enum struct EANS_DetectRange_NotifyDetectionFrequencyType : uint8
{
  Notify_FrameBase UMETA(DisplayName = "Based on Montage keyframe"),
  Notify_TimeBase UMETA(DisplayName = "Based on Time"),
};

/**
 * 
 */
UCLASS(editinlinenew, const, hidecategories = Object, collapsecategories, MinimalAPI, meta = (DisplayName = "Detect Range"))
class UAnimNotifyState_DetectRange : public UAnimNotifyState
{
	GENERATED_BODY()

public:
  class FDetectTickObject
  {
    public:
      virtual ~FDetectTickObject() = default;

      virtual void TickDetection(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) = 0;

      FOnDetectionNotify DetectionNotify;
  };

protected:

  /**Start of UAnimNotifyState Interface */
  UE_API virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	UE_API virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	UE_API virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
  UE_API FString GetNotifyName_Implementation() const override;
	/**End of UAnimNotifyState Interface */

  UPROPERTY(EditAnywhere, Category = "RangeDetection")
  TObjectPtr<UPrimitiveDetectorData> RangeData;

  UPROPERTY(EditAnywhere, Category = "RangeDetection")
  FName SocketName;

  UPROPERTY(EditAnywhere, Category = "RangeDetection")
  FVector LocationOffset;

  UPROPERTY(EditAnywhere, Category = "RangeDetection")
  FRotator RotationOffset;

  UPROPERTY(EditAnywhere, Category = "RangeDetection")
  EANS_DetectRange_NotifyDetectionType DetectionType;

  UPROPERTY(EditAnywhere, Category = "RangeDetection", meta = (EditCondition = "DetectionType == EANS_DetectRange_NotifyDetectionType::NotifyDuringHit", EditConditionHides))
  EANS_DetectRange_NotifyDetectionFrequencyType FrequencyType;

  UPROPERTY(EditAnywhere, Category = "RangeDetection", meta = (EditCondition = "DetectionType == EANS_DetectRange_NotifyDetectionType::NotifyDuringHit && FrequencyType == EANS_DetectRange_NotifyDetectionFrequencyType::Notify_FrameBase", EditConditionHides))
  int32 FrameInterval;

  UPROPERTY(EditAnywhere, Category = "RangeDetection", meta = (EditCondition = "DetectionType == EANS_DetectRange_NotifyDetectionType::NotifyDuringHit && FrequencyType == EANS_DetectRange_NotifyDetectionFrequencyType::Notify_TimeBase", EditConditionHides))
  float TimeInterval;

  UPROPERTY(EditAnywhere, Category = "Auto Destroy")
  bool bDestroyAtEnd = true;

#if WITH_EDITORONLY_DATA

  UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "RangeData != nullptr", DisplayName = "Draw Debug Line if activating"))
  bool bDrawDebugDuringActivation = true;

#endif

private:
  bool ValidateParameters(USkeletalMeshComponent* MeshComp) const;

  void NotifyAbility(USkeletalMeshComponent* MeshComp, UAnimSequenceBase * Animation);

#if WITH_EDITOR

  void DrawDebugRange(USceneComponent* RootComponent);

#endif

private:
  TUniquePtr<FDetectTickObject> m_detectTickObject;

  // TODO 
  TSet<AActor*> m_notifiedActors;
   
};

#undef UE_API
