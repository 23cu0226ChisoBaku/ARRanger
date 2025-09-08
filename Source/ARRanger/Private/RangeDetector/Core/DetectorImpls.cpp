#include "RangeDetector/Core/DetectorImpls.h"

#include "RangeDetector/Core/PrimitiveDetectorData.h"
#include "RangeDetector/DetectorDatas/ConeCollisionDataAsset.h"
#include "RangeDetector/Utils/CollisionTraceFunctionLibrary.h"

#if WITH_EDITOR
#include "SceneManagement.h" // Use of FPrimitiveDrawInterface
#endif 

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
      if (const UWorld* world = GEngine->GetCurrentPlayWorld())
      {
        DebugDrawRange(world, InOriginPosition, InColor);
      }
      else
      {
        UE_LOG(LogTemp, Warning, TEXT("Can not draw range if world is invalid or non-play world"));
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

  void UConeCollisionDataAsset::DebugDrawRange(FPrimitiveDrawInterface* PDI, const FVector& Location) const
  {
    if ((GEngine != nullptr) && (GEngine->WireframeMaterial != nullptr))
    {
      // TODO Temporary

      FMatrix coneMatrix = FRotationMatrix::Make(LocalDirectionRotator);
      coneMatrix.SetOrigin(Location + CenterPositionOffset);
      coneMatrix = coneMatrix.ApplyScale(Height);
      const FMaterialRenderProxy* materialProxy = GEngine->WireframeMaterial->GetRenderProxy();
      const float angleToRad = FMath::DegreesToRadians(ConeAngle / 2.0f);

      ::DrawCone(
                  PDI, 
                  coneMatrix, 
                  angleToRad, 
                  angleToRad, 
                  NumSides,
                  true,       // bDrawSideLines
                  FLinearColor::Red,
                  materialProxy,
                  SDPG_World
                );
    }
  }

#endif // WITH_EDITOR