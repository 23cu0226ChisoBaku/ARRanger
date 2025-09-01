#include "RangeDetector/Core/DetectorImpls.h"

#include "RangeDetector/Core/PrimitiveDetectorData.h"
#include "RangeDetector/DetectorDatas/ConeCollisionDataAsset.h"
#include "RangeDetector/Utils/CollisionTraceFunctionLibrary.h"

namespace ARRanger
{

namespace Detector
{
  int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const UPrimitiveDetectorData& InData, TArray<TObjectPtr<AActor>>& OutResult)
  {
    return 0;
  }

  int32 DetectTargetsImpl(UWorld* World, AActor* OriginActor, const UConeCollisionDataAsset& InData, TArray<TObjectPtr<AActor>>& OutResult)
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

    const FVector startPosition = InData.CenterPosition + InData.CenterPositionOffset + (OriginActor != nullptr ? OriginActor->GetActorLocation() : FVector::ZeroVector);

    // TODO Channelに変更する
    const int32 resultNum = UCollisionTraceFunctionLibrary::SweepConeMulti(
                              World, 
                              startPosition,
                              InData.LocalDirectionRotator,
                              InData.Height,
                              InData.ConeAngle / 2.0f,
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
}

}

DEFINE_PRIMITIVE_DETECTOR(UConeCollisionDataAsset)

/**
 * Implementations of Debug function
 */
#if WITH_EDITOR

  void UPrimitiveDetectorData::DebugDrawRange(const FVector& InOriginPosition, const FColor& InColor) const
  {
    if (GEngine != nullptr)
    {
      if (const UWorld* playWorld = GEngine->GetCurrentPlayWorld())
      {
        DebugDrawRange(playWorld, InOriginPosition, InColor);
      }
      else
      {
        UE_LOG(LogTemp, Warning, TEXT("Can not draw range in invalid world"));
      }
    }

  }

  void UConeCollisionDataAsset::DebugDrawRange(const UWorld* InWorld, const FVector& InOriginPosition, const FColor& InColor) const
  {
    /**Ensure the world is valid */
    check(InWorld != nullptr);
    ::DrawDebugCone(InWorld, 
                    InOriginPosition, 
                    LocalDirectionRotator.Vector(), 
                    Height,
                    FMath::DegreesToRadians(ConeAngle / 2.0f),
                    FMath::DegreesToRadians(ConeAngle / 2.0f),
                    NumSides,
                    InColor);
  }

#endif // WITH_EDITOR