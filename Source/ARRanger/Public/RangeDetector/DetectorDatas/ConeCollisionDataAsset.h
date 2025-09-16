//*************************************************
// ConeCollision のデータアセット
//*************************************************

#pragma once

#include "Engine/DataAsset.h"

#include "RangeDetector/Core/PrimitiveDetectorData.h"

#include "ConeCollisionDataAsset.generated.h"

#define UE_API ARRANGER_API

UCLASS(Const)
class UConeCollisionDataAsset : public UPrimitiveDetectorData
{
	GENERATED_BODY()
	
public:

  /**円錐の向き */
  UPROPERTY(EditDefaultsOnly)
  FRotator LocalDirectionRotator;

  /*コリジョン範囲(円錐の高さ)*/
  UPROPERTY(EditDefaultsOnly)							
  float Height;         

  /*コリジョンの傾き*/
  UPROPERTY(EditDefaultsOnly)
  float ConeAngle;

  /**円錐の底面の辺の数 */
  UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 3, ClampMax = 100))
  int32 NumSides = 24;

  /**Declaration of Visitor pattern */
  DECLARE_PRIMITIVE_DETECTOR(UConeCollisionDataAsset);

#if WITH_EDITOR

private:
  UE_API virtual void DebugDrawRange(const UWorld* InWorld, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const override;

  UE_API virtual void DebugDrawRange(FPrimitiveDrawInterface* PDI, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const override;

  UE_API virtual void DebugDrawRange(ULineBatchComponent* LineBatch, const FVector& InOriginLocation, const FRotator& InOriginRotation, const FVector& InOriginScale3D) const override;

#endif // WITH_EDITOR

};

#undef UE_API