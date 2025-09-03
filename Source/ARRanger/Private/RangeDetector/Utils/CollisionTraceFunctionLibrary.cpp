// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeDetector/Utils/CollisionTraceFunctionLibrary.h"

int32 UCollisionTraceFunctionLibrary::SweepConeMulti(UWorld* World, const FVector& StartPosition, const FRotator& Direction, float ConeHeight, float ConeHalfAngle, const TArray<TObjectPtr<AActor>>& ActorsToIgnore, TArray<FHitResult>& OutHitResults)
{
  if (World == nullptr)
  {
    return 0;
  }

  constexpr bool bTraceComplex = true;

  OutHitResults.Reset();

  FCollisionQueryParams queryParams{SCENE_QUERY_STAT(SweepConeMulti), bTraceComplex};
  queryParams.bReturnPhysicalMaterial = true;
  queryParams.AddIgnoredActors(ActorsToIgnore);

  FCollisionObjectQueryParams objQueryParams{};
  objQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
  objQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
  objQueryParams.AddObjectTypesToQuery(ECC_Pawn);
  objQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

  TArray<FHitResult> HitResults{};
  const FVector endPosition = StartPosition + (Direction.Vector() * ConeHeight);
  const float coneHalfAngleRad = FMath::DegreesToRadians(ConeHalfAngle);
  const float coneBaseRadius = FMath::Tan(coneHalfAngleRad) * ConeHeight;

  // Use Sphere as an imaginary cone
  const FCollisionShape imaginarySphereSweep = FCollisionShape::MakeSphere(coneBaseRadius);
  World->SweepMultiByObjectType(HitResults, StartPosition, endPosition, Direction.Quaternion(), objQueryParams, imaginarySphereSweep, queryParams);

  // Filter results that is not in cone
  for (const FHitResult& hitResult : HitResults)
  {
    const FVector hitResultDirection = (hitResult.ImpactPoint - StartPosition).GetSafeNormal();
    const double dot = FVector::DotProduct(Direction.Vector(), hitResultDirection);
    const double angle = FMath::Acos(dot);

    // Hit is outside the angle of cone
    if (angle > coneHalfAngleRad)
    {
      continue;
    }

    const double distance = (hitResult.ImpactPoint - StartPosition).Length();
    const double lengthAtAngle = ConeHeight / FMath::Cos(angle);
    
    // Ignore hit beyond the cone's base
    if (distance > lengthAtAngle)
    {
      continue;
    }

    OutHitResults.Add(hitResult);
  }

  return OutHitResults.Num();
}