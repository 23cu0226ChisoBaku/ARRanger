// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "CollisionTraceFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ARRANGER_API UCollisionTraceFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
  [[nodiscard]] static int32 SweepConeMulti(UWorld* World, const FVector& StartPosition, const FRotator& Direction, float ConeHeight, float ConeHalfAngle, const TArray<TObjectPtr<AActor>>& ActorsToIgnore, TArray<FHitResult>& OutHitResults);
	
};
