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
  int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, const UPrimitiveDetectorData& InData, TArray<TObjectPtr<AActor>>& OutResult)
  {
    return 0;
  }

  int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, const UConeCollisionDataAsset& InData, TArray<TObjectPtr<AActor>>& OutResult)
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

    // TODO Channelに変更する
    const int32 resultNum = UCollisionTraceFunctionLibrary::SweepConeMulti(
                              World, 
                              startPosition,
                              finalRotation,
                              InData.Height,
                              halfAngle,
                              ignoreActors,
                              hitResults);

    if (resultNum < 1)
    {
      UE_LOG(LogTemp, Warning, TEXT("Cone hit nothing"));
    }

    // Sweep multi will hit all components.So we should ignore same actor adding into array
    /**同じActorの複数のコンポネントが追加される可能性があるため、同じActorを一つだけ入れる */
    for (int32 idx = 0; idx < resultNum; ++idx)
    {
      OutResult.AddUnique(hitResults[idx].GetActor());
    }

    return resultNum;
  }

  int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, const UCapsuleDetectorData& InData, TArray<TObjectPtr<AActor>>& OutResult)
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

    TArray<TEnumAsByte<EObjectTypeQuery>> objTypes{};
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

    const bool bHit = UKismetSystemLibrary::CapsuleOverlapActors(
                        OriginActor,
                        InOriginLocation,
                        InData.CapsuleRadius,
                        InData.CapsuleHalfHeight,
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

  int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D, const USphereDetectorData& InData, TArray<TObjectPtr<AActor>>& OutResult)
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

    TArray<TEnumAsByte<EObjectTypeQuery>> objTypes{};
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

    const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
                        OriginActor,
                        InOriginLocation,
                        InData.SphereRadius,
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
