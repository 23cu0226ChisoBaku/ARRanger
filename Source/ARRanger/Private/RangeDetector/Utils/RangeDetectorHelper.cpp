// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeDetector/Utils/RangeDetectorHelper.h"

#include "RangeDetector/GameFramework/RangeDetectorComponent.h"
#include "RangeDetector/Core/PrimitiveDetectorData.h"

namespace RangeDetector::Private
{
  bool ValidateParamaters_DetectorAssetEntry(const FDetectorAssetEntry& Entry);
}

URangeDetectorComponent* URangeDetectorHelper::AttachRangeDetector(const FDetectorAssetEntry& Entry, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroy)
{
  // TODO Need implementation

  // Check Entry Validation
  if (!RangeDetector::Private::ValidateParamaters_DetectorAssetEntry(Entry))
  {
    return nullptr;
  }

  if (AttachToComponent == nullptr)
  {
    UE_LOG(LogTemp, Error, TEXT("URangeDetectorHelper::AttachRangeDetector: Invalid AttachComponent specified! Trying to spawn detector [%s]"), *Entry.DetectorData->GetName());
    return nullptr;
  }

  UWorld* const world = AttachToComponent->GetWorld();
  // TODO Server code
  if ((world != nullptr) && world->IsNetMode(NM_DedicatedServer))
  {
    return nullptr;
  }

  URangeDetectorComponent* RDC = NewObject<URangeDetectorComponent>(AttachToComponent, NAME_None);
  if (RDC != nullptr)
  {
    RDC->SetupAttachment(AttachToComponent, AttachPointName);

    if (LocationType == EAttachLocation::KeepWorldPosition)
    {
      const FTransform parentWorldTransform = AttachToComponent->GetSocketTransform(AttachPointName);
      const FTransform componentWorldTransform{Rotation, Location, Scale};
      const FTransform relativeTransform = componentWorldTransform.GetRelativeTransform(parentWorldTransform);
      RDC->SetRelativeLocation_Direct(relativeTransform.GetLocation());
      RDC->SetRelativeRotation_Direct(relativeTransform.GetRotation().Rotator());
      RDC->SetRelativeScale3D_Direct(relativeTransform.GetScale3D());
    }
    else
    {
      RDC->SetRelativeLocation_Direct(Location);
      RDC->SetRelativeRotation_Direct(Rotation);

      if (LocationType == EAttachLocation::SnapToTarget)
      {
        const FTransform parentWorldTransform = AttachToComponent->GetSocketTransform(AttachPointName);
        RDC->SetRelativeScale3D_Direct(Scale * parentWorldTransform.GetSafeScaleReciprocal(parentWorldTransform.GetScale3D()));
      }
      else
      {
        RDC->SetRelativeScale3D_Direct(Scale);
      }
    }

    // Activate component
    RDC->AddNewDetector(Entry); 
    RDC->RegisterComponentWithWorld(world);

    RDC->bStopWhenOwnerDestroyed = bStopWhenAttachedToDestroy;
  }

  return RDC;
} 

namespace RangeDetector::Private
{
  bool ValidateParamaters_DetectorAssetEntry(const FDetectorAssetEntry& Entry)
  {
    bool result = true;

    if (Entry.DetectorData == nullptr)
    {
      result = false;
    }
    else
    {
      switch (Entry.TargetInfo.Type)
      {
        case EDetectorTargetType::Actor:
        {
          if (Entry.TargetInfo.TargetActor == nullptr)
          {
            result = false;
          }
        }
        break;

        case EDetectorTargetType::Interface:
        {
          if (Entry.TargetInfo.TargetInterface == nullptr)
          {
            result = false;
          }
        }
        break;
      }
    }

    return result;
  }
}