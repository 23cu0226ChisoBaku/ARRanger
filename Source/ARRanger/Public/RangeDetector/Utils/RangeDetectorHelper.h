// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "RangeDetectorHelper.generated.h"

class URangeDetectorComponent;
struct FDetectorAssetEntry;

#define UE_API ARRANGER_API

UCLASS()
class URangeDetectorHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
  UFUNCTION(BlueprintCallable, Category = "RangeDetection")
  static UE_API URangeDetectorComponent* AttachRangeDetector(const FDetectorAssetEntry& Entry, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroy = true); 
	
};

#undef UE_API