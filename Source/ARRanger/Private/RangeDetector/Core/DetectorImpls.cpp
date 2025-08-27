#include "RangeDetector/Core/DetectorImpls.h"

#include "RangeDetector/Core/PrimitiveDetectorData.h"
#include "RangeDetector/DetectorDatas/ConeCollisionDataAsset.h"
#include "Kismet/KismetSystemLibrary.h"

namespace ARRanger
{

namespace Detector
{
  int32 DetectTargetsImpl(const UPrimitiveDetectorData& InData, TArray<AActor*>& OutResult)
  {
    return 0;
  }

  int32 DetectTargetsImpl(const UConeCollisionDataAsset& InData, TArray<AActor*>& OutResult)
  {
    return 0;
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