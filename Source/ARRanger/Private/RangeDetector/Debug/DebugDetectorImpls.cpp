#include "RangeDetector/Core/DetectorImpls.h"

#include "RangeDetector/Core/PrimitiveDetectorData.h"
#include "RangeDetector/DetectorDatas/ConeCollisionDataAsset.h"
#include "RangeDetector/DetectorDatas/CapsuleDetectorData.h"
#include "RangeDetector/DetectorDatas/SphereDetectorData.h"

#include "RangeDetector/Utils/CollisionTraceFunctionLibrary.h"

#if WITH_EDITOR
#include "SceneManagement.h" // Use of FPrimitiveDrawInterface
#include "Components/LineBatchComponent.h"
#endif 

/**
 * Implementations of Debug function
 */
#if WITH_EDITOR

  void UPrimitiveDetectorData::DebugDrawRange(const UObject* InWorldContextObject, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const
  {
    if (GEngine != nullptr)
    {
      if (const UWorld* world = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
      {
        DebugDrawRange(world, InOriginLocation, InOriginRotation, InOriginScale3D);
      }
    }
  }

  void UConeCollisionDataAsset::DebugDrawRange(const UWorld* InWorld, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const
  {
    /**Ensure the world is valid */
    check(InWorld != nullptr);

    const FVector originLoc = InOriginLocation + CenterPositionOffset;
    const FVector direction = (InOriginRotation + LocalDirectionRotator).Vector();
    const float halfAngleRad = FMath::DegreesToRadians(ConeAngle / 2.0f);
    const float scale = InOriginScale3D.GetMax();

    ::DrawDebugCone(InWorld, 
                    originLoc, 
                    direction, 
                    Height * scale,
                    halfAngleRad,
                    halfAngleRad,
                    NumSides,
                    DebugLineColor.ToFColorSRGB());
  }

  void UConeCollisionDataAsset::DebugDrawRange(FPrimitiveDrawInterface* PDI, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const
  {
      const double halfAngle = static_cast<double>(ConeAngle) / 2.0;

      const FVector originLoc = InOriginLocation + CenterPositionOffset;
      const FRotator originRot = InOriginRotation + LocalDirectionRotator;
      const FVector originScale3D = InOriginScale3D;
      const FTransform originTransform{originRot, originLoc, originScale3D};
      const float scale = InOriginScale3D.GetMax();

      ::DrawWireSphereCappedCone(
                                  PDI,
                                  originTransform,
                                  Height * scale,
                                  halfAngle,
                                  NumSides,
                                  1,           // Draw arc between every vertex
                                  1,           // Num of lines to draw arc
                                  DebugLineColor,
                                  SDPG_World
                                );

  }
  
  void UConeCollisionDataAsset::DebugDrawRange(ULineBatchComponent* LineBatch, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const 
  {
    if (LineBatch != nullptr)
    {
      const FVector coneOrigin = InOriginLocation + CenterPositionOffset;
      const FVector coneDirection = (InOriginRotation + LocalDirectionRotator).Vector();
      const float halfAngleToRad = FMath::DegreesToRadians(ConeAngle) / 2.0f;
      const float scale = InOriginScale3D.GetMax();

      LineBatch->DrawCone(
                          coneOrigin,
                          coneDirection,
                          Height * scale,
                          halfAngleToRad,
                          halfAngleToRad,
                          NumSides,
                          DebugLineColor,
                          0.0f,
                          SDPG_World,
                          0.0f             // TODO
                         );
    }
  }

  void UCapsuleDetectorData::DebugDrawRange(const UWorld* InWorld, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const
  {
    /**Ensure the world is valid */
    check(InWorld != nullptr);

    const FVector originLoc = InOriginLocation + CenterPositionOffset;
    const FQuat originRot = (InOriginRotation + RotationOffset).Quaternion();
    const float scale = InOriginScale3D.GetMax();

    ::DrawDebugCapsule(
                        InWorld,
                        originLoc,
                        CapsuleHalfHeight * scale,
                        CapsuleRadius * scale,
                        originRot,
                        DebugLineColor.ToFColorSRGB()
                        );
  }

  void UCapsuleDetectorData::DebugDrawRange(FPrimitiveDrawInterface* PDI, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const
  {

  }

  void UCapsuleDetectorData::DebugDrawRange(ULineBatchComponent* LineBatch, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const
  {
    if (LineBatch != nullptr)
    {
      const FVector originLoc = InOriginLocation + CenterPositionOffset;
      const FQuat originRot = (InOriginRotation + RotationOffset).Quaternion();
      const float scale = InOriginScale3D.GetMax();

      LineBatch->DrawCapsule(
                              originLoc,
                              CapsuleHalfHeight * scale,
                              CapsuleRadius * scale,
                              originRot,
                              DebugLineColor,
                              0.0f,
                              SDPG_World,
                              0.0f              // TODO
                            );
    }
  }

  void USphereDetectorData::DebugDrawRange(const UWorld* InWorld, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const
  {
    check(InWorld != nullptr);

    const int32 segments = 8;
    const FVector originLoc = InOriginLocation + CenterPositionOffset;
    const float scale = InOriginScale3D.GetMax();

    ::DrawDebugSphere(
                      InWorld,
                      originLoc,
                      SphereRadius * scale,
                      segments,
                      DebugLineColor.ToFColorSRGB()
                     );
  }

  void USphereDetectorData::DebugDrawRange(FPrimitiveDrawInterface* PDI, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const
  {

  }

  void USphereDetectorData::DebugDrawRange(ULineBatchComponent* LineBatch, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const
  {
    if (LineBatch != nullptr)
    {
      const int32 segments = 8;
      const FVector originLoc = InOriginLocation + CenterPositionOffset;
      const float scale = InOriginScale3D.GetMax();
      
      LineBatch->DrawSphere(
                              originLoc,
                              SphereRadius * scale,
                              segments,
                              DebugLineColor,
                              0.0f,
                              SDPG_World,
                              0.0f
      );
    }
  }

#endif // WITH_EDITOR