// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/AnimNotifyState_DetectRange.h"

#include "RangeDetector/GameFramework/RangeDetectorComponent.h"
#include "RangeDetector/Core/RangeDetector.h"
#include "RangeDetector/Core/PrimitiveDetectorData.h"
#include "RangeDetector/Utils/RangeDetectorHelper.h"

#include "ActionAbilities/ARAbilitySystemComponent.h"
#include "ActionAbilities/Abilities/IARGameplayAbilityNotifyInterface.h"

#if WITH_EDITOR
#include "Components/LineBatchComponent.h"
#endif

class FDetectTickObject_FrameBase : public UAnimNotifyState_DetectRange::FDetectTickObject
{
public:

  FDetectTickObject_FrameBase(int32 InStartFrame, int32 InFrameInterval);
  void TickDetection(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;

  const int32 FrameInterval;

  int32 CurrentFrame = 0;

  int32 FrameCnt = 0;
};

class FDetectTickObject_TimeBase : public UAnimNotifyState_DetectRange::FDetectTickObject
{
public:

  FDetectTickObject_TimeBase(float InTimeInterval);
  void TickDetection(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;

  const float TimeInterval;

  float TimeCnt;

};

void UAnimNotifyState_DetectRange::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
  Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

  if (ValidateParameters(MeshComp))
  {
    if (RangeData != nullptr)
    {
      // TODO
      FDetectorAssetEntry DAE{};
      DAE.DetectorData = RangeData;
      DAE.Target.TargetType = EDetectorTargetType::Actor;
      DAE.Target.TargetActor = AActor::StaticClass();
      DAE.Priority = 0;
      
      URangeDetectorComponent* RDC = URangeDetectorHelper::AttachRangeDetector(DAE, MeshComp, SocketName, LocationOffset, RotationOffset, FVector::OneVector, EAttachLocation::KeepRelativeOffset, true);

      if (!m_detectTickObject.IsValid() && DetectionType == EANS_DetectRange_NotifyDetectionType::NotifyDuringHit)
      {
        using enum EANS_DetectRange_NotifyDetectionFrequencyType;
        switch (FrequencyType)
        {
          case Notify_FrameBase:
          {
            // TODO Currently we do not use startFrame
            const int32 startFrame = 0;
            m_detectTickObject = ::MakeUnique<FDetectTickObject_FrameBase>(startFrame, FrameInterval);
          }
          break;

          case Notify_TimeBase:
          {
            m_detectTickObject = ::MakeUnique<FDetectTickObject_TimeBase>(TimeInterval);
          }
          break;
        }

        if (m_detectTickObject.IsValid())
        {
          m_detectTickObject->DetectionNotify.BindUObject(this, &UAnimNotifyState_DetectRange::NotifyAbility);
        }
      }

#if WITH_EDITORONLY_DATA

      if (RDC != nullptr)
      {
        RDC->bDrawDebugRange = bDrawDebugDuringActivation;
      }

#endif // WITH_EDITORONLY_DATA  
    }
  }

  m_notifiedActors.Reset();
}

void UAnimNotifyState_DetectRange::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
  Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

  // Notify in tick object if its valid
  if (m_detectTickObject.IsValid())
  {
    m_detectTickObject->TickDetection(MeshComp, Animation, FrameDeltaTime);
  }
  // Get result and notify once
  else
  {
    // Try to notify abilities
    NotifyAbility(MeshComp, Animation);
  }

#if WITH_EDITORONLY_DATA

  if (bDrawDebugDuringActivation)
  {
    #if WITH_EDITOR
      DrawDebugRange(MeshComp);
    #endif
  }

#endif
}

void UAnimNotifyState_DetectRange::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference)
{
  TArray<USceneComponent*> children{};
  MeshComp->GetChildrenComponents(false, children);
  
  for (USceneComponent* child : children)
  {
    URangeDetectorComponent* RDC = ::Cast<URangeDetectorComponent>(child);
    if (RDC != nullptr)
    {
      const bool bSocketMatch = RDC->GetAttachSocketName() == SocketName;
      const bool bRangeDataMatch = RDC->HasRangeData(RangeData);
      
      if (bSocketMatch && bRangeDataMatch)
      {
        RDC->RemoveDetector(RangeData);
        
        if (bDestroyAtEnd && RDC->IsDetectorEmpty())
        {
          RDC->DestroyComponent();
        }
        else
        {
          RDC->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        }
        
        break;
      }
    }
  }

  Super::NotifyEnd(MeshComp, Animation, EventReference);

  m_notifiedActors.Reset();

#if WITH_EDITOR

  if (MeshComp->GetWorld() != nullptr)
  {
    if (bDrawDebugDuringActivation)
    {
      ULineBatchComponent* lineBatch = MeshComp->GetWorld()->GetLineBatcher(UWorld::ELineBatcherType::WorldPersistent);

      // Flush all debug lines drawn in NotifyTick
      if (lineBatch != nullptr)
      {
        lineBatch->Flush();
      }
    }    
  }

#endif
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

void UAnimNotifyState_DetectRange::NotifyAbility(USkeletalMeshComponent* MeshComp, UAnimSequenceBase * Animation)
{
  if (MeshComp == nullptr)
  {
    return;
  }

  TArray<USceneComponent*> children{};
  MeshComp->GetChildrenComponents(false, children);

  for (USceneComponent* child : children)
  {
    URangeDetectorComponent* RDC = ::Cast<URangeDetectorComponent>(child);
    if ((RDC == nullptr) || !RDC->HasRangeData(RangeData))
    {
      continue;
    } 

    FRangeDetectorEvaluationResult result{};
    const int32 resultCnt = RDC->GetResultByRangeData(RangeData, result);
    if (resultCnt <= 0)
    {
      continue;
    }

    // Notify Ability
    UARAbilitySystemComponent* ARASC = UARAbilitySystemComponent::FindARAbilitySystemComponent(MeshComp->GetOwner());
    if (ARASC != nullptr)
    {
      const bool bIsOnlyNotifyOnce = DetectionType == EANS_DetectRange_NotifyDetectionType::NotifyOnce;

      const TArray<FGameplayAbilitySpec>& allActivatableAbilites = ARASC->GetActivatableAbilities();
      for (const FGameplayAbilitySpec& activatableAbility : allActivatableAbilites)
      {
        if (activatableAbility.IsActive())
        {
          if (IARGameplayAbilityNotifyInterface* notifyInterface = ::Cast<IARGameplayAbilityNotifyInterface>(activatableAbility.GetPrimaryInstance()))
          {
            TArray<FGANotify_ImpactResult> impactResults{};

            for (const auto& detectedActor : result.DetectedActors)
            {
              // TODO Only for notify once use. Maybe we should need some other solution
              if (bIsOnlyNotifyOnce && m_notifiedActors.Contains(detectedActor))
              {
                continue;
              }

              m_notifiedActors.Emplace(detectedActor);

              FGANotify_ImpactResult impactResult{
                .SourceActor = MeshComp->GetOwner(),
                .HitActor = detectedActor,
                .OccurrenceComp = RDC,
                .ImpactLocation = RDC->GetComponentLocation()
              };

              impactResults.Emplace(impactResult);
            }
            
            notifyInterface->GANotify_ImpactResult(MeshComp, Animation, impactResults);
          }
        }
      }
    } 
  }
}

#if WITH_EDITOR

void UAnimNotifyState_DetectRange::DrawDebugRange(USceneComponent* RootComponent)
{
  if (RootComponent != nullptr)
  {
    const UWorld* world = RootComponent->GetWorld();
    if (world != nullptr)
    {
      if (bDrawDebugDuringActivation)
      {
        ULineBatchComponent* lineBatch = world->GetLineBatcher(UWorld::ELineBatcherType::WorldPersistent);

        if (lineBatch != nullptr)
        {
          lineBatch->Flush();
          TArray<USceneComponent*> children{};
          RootComponent->GetChildrenComponents(false, children);
          for (USceneComponent* child : children)
          {
            if (URangeDetectorComponent* RDC = ::Cast<URangeDetectorComponent>(child))
            {
              RDC->ED_DrawWithLineBatchComp(lineBatch);
            }
          }
        }
      }    
    }
  }
}

#endif

FDetectTickObject_FrameBase::FDetectTickObject_FrameBase(int32 InStartFrame, int32 InFrameInterval)
  : FrameInterval{InFrameInterval}
  , CurrentFrame{InStartFrame}
  , FrameCnt{InFrameInterval}
{ }

void FDetectTickObject_FrameBase::TickDetection(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
  // TODO Add Frame count each time tick detection is called
  FrameCnt += 1;
  if (FrameCnt >= FrameInterval)
  {
    DetectionNotify.ExecuteIfBound(MeshComp, Animation);
    FrameCnt = 0;
  }
}

FDetectTickObject_TimeBase::FDetectTickObject_TimeBase(float InTimeInterval)
  : TimeInterval{InTimeInterval}
  , TimeCnt{InTimeInterval}
{ }

void FDetectTickObject_TimeBase::TickDetection(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
  TimeCnt += FrameDeltaTime;
  if (TimeCnt >= TimeInterval)
  {
    DetectionNotify.ExecuteIfBound(MeshComp, Animation);
    TimeCnt = 0.0f;
  }
}