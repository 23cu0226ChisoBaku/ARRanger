#include "RangeDetector/Core/DetectorImpls.h"

#include "RangeDetector/Core/PrimitiveDetectorData.h"
#include "RangeDetector/DetectorDatas/ConeCollisionDataAsset.h"
#include "RangeDetector/DetectorDatas/CapsuleDetectorData.h"
#include "RangeDetector/DetectorDatas/SphereDetectorData.h"

#include "RangeDetector/Utils/CollisionTraceFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

namespace ARRanger
{

namespace Detector
{
  int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, const UPrimitiveDetectorData& InData, TArray<AActor*>& OutResult)
  {
    return 0;
  }

  int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, const UConeCollisionDataAsset& InData, TArray<AActor*>& OutResult)
  {
    if (World == nullptr)
    {
      return 0;
    }

    OutResult.Reset();

    TArray<FHitResult> hitResults{};

    // Ignore origin actor
    TArray<TObjectPtr<AActor>> ignoreActors{};
    ignoreActors.Add(OriginActor);

    const FVector startPosition = InOriginLocation + InData.CenterPositionOffset;
    const FRotator finalRotation = InOriginRotation + InData.LocalDirectionRotator;
    const float halfAngle = InData.ConeAngle / 2.0f;
    const float scale = InOriginScale3D.GetMax();

    // TODO Channelに変更する
    const int32 resultNum = UCollisionTraceFunctionLibrary::SweepConeMulti(
                              World, 
                              startPosition,
                              finalRotation,
                              InData.Height * scale,
                              halfAngle,
                              ignoreActors,
                              hitResults);

    if (resultNum < 1)
    {
      UE_LOG(LogTemp, Warning, TEXT("Cone hit nothing"));
    }

    for (const FHitResult& hitResult : hitResults)
    {
      OutResult.AddUnique(hitResult.GetActor());
    }

    return resultNum;
  }

  int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, const UCapsuleDetectorData& InData, TArray<AActor*>& OutResult)
  {
    check(World != nullptr);
    if (World == nullptr)
    {
      return 0;
    }

    OutResult.Reset();
    TArray<AActor*> hitActors{};
    // Ignore origin actor
    TArray<TObjectPtr<AActor>> ignoreActors{};
    ignoreActors.Add(OriginActor);

    const FVector startLoc = InOriginLocation + InData.CenterPositionOffset;
    const float scale = InOriginScale3D.GetMax();

    TArray<TEnumAsByte<EObjectTypeQuery>> objTypes{};
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

    const bool bHit = UKismetSystemLibrary::CapsuleOverlapActors(
                        OriginActor,
                        startLoc,
                        InData.CapsuleRadius * scale,
                        InData.CapsuleHalfHeight * scale,
                        objTypes,
                        nullptr,
                        ignoreActors,
                        hitActors
                      );
    
    if (bHit)
    {
      for (AActor* actor : hitActors)
      {
        OutResult.AddUnique(actor);
      }
    }

    return OutResult.Num();
  }

  int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, const USphereDetectorData& InData, TArray<AActor*>& OutResult)
  {
    check(World != nullptr);
    if (World == nullptr)
    {
      return 0;
    }

    OutResult.Reset();
    TArray<AActor*> hitActors{};
    // Ignore origin actor
    TArray<TObjectPtr<AActor>> ignoreActors{};
    ignoreActors.Add(OriginActor);

    const FVector originLoc = InOriginLocation + InData.CenterPositionOffset;
    const float scale = InOriginScale3D.GetMax();

    TArray<TEnumAsByte<EObjectTypeQuery>> objTypes{};
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

    const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
                        OriginActor,
                        InOriginLocation,
                        InData.SphereRadius * scale,
                        objTypes,
                        nullptr,
                        ignoreActors,
                        hitActors
                      );
    if (bHit)
    {
      for (AActor* hitActor : hitActors)
      {
        OutResult.AddUnique(hitActor);
      }
    }

    return OutResult.Num();
  }
} // namespace ARRanger::Detector

} // namespace ARRanger

DEFINE_PRIMITIVE_DETECTOR(UConeCollisionDataAsset)
DEFINE_PRIMITIVE_DETECTOR(UCapsuleDetectorData)
DEFINE_PRIMITIVE_DETECTOR(USphereDetectorData)
